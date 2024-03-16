#include "parser.h"

OgreScriptLSP::Parser::Parser() {
    scanner = std::make_unique<Scanner>();
}

void OgreScriptLSP::Parser::loadScript(const std::string &fileUri, const std::string &code) {
    exceptions.clear();
    this->uri = fileUri;
    scanner->loadScript(uriToPath(fileUri), code);
    tokens = scanner->parse();
    exceptions.insert(exceptions.end(), scanner->exceptions.begin(), scanner->exceptions.end());
    currentToken = 0;
    script.reset();
    script = std::make_unique<MaterialScriptAst>(fileUri);
}

std::string OgreScriptLSP::Parser::uriToPath(const std::string &uri) {
    if (uri.starts_with("file://")) {
        return uri.substr(7);
    }
    return uri;
}

void OgreScriptLSP::Parser::parse(const std::string &fileUri) {
    loadScript(fileUri);
    declarations.clear();
    parse();
}

void OgreScriptLSP::Parser::parse() {
    initSwap();

    bool recuperate = false;
    while (!isEof()) {
        consumeEndLines();
        TokenValue tk = getToken();
        try {
            if (tk.tk == fragment_program_tk || tk.tk == vertex_program_tk || tk.tk == geometry_program_tk ||
                tk.tk == tessellation_hull_program_tk || tk.tk == tessellation_domain_program_tk ||
                tk.tk == compute_program_tk) {
                recuperate = false;
                program(script.get());
            } else if (tk.tk == material_tk) {
                recuperate = false;
                material(script.get());
            } else if (tk.tk == abstract_tk) {
                recuperate = false;
                abstract(script.get());
            } else if (tk.tk == sampler_tk) {
                recuperate = false;
                sampler(script.get());
            } else if (tk.tk == import_tk) {
                recuperate = false;
                importBlock(script.get());
            } else if (tk.tk == shared_params_tk) {
                recuperate = false;
                sharedParams(script.get());
            } else if (!recuperate) {
                exceptions.push_back(ParseException(INVALID_TOKEN, tk.toRange()));
                recuperate = true;
            }
        } catch (ParseException &e) {
            exceptions.push_back(e);
            recuperate = true;
            continue;
        }
        if (recuperate) {
            nextTokenAndConsumeEndLines();
        }
    }
}

// IMPORT STATEMENT
void OgreScriptLSP::Parser::importBlock(MaterialScriptAst *scriptAst) {
    auto importAst = std::make_unique<ImportAst>();

    consumeToken(import_tk);

    importAst->imported = getToken();
    if (getToken().tk == asterisk_tk) {
        nextToken();
    } else {
        consumeToken(identifier, NOT_VALID_IMPORT);
    }

    consumeToken(from_tk, NOT_VALID_IMPORT);

    if (getToken().tk != string_literal && getToken().tk != identifier) {
        throw ParseException(NOT_VALID_IMPORT, getToken().toRange());
    }

    importAst->file = getToken();
    scriptAst->imports.push_back(std::move(importAst));
    nextTokenAndConsumeEndLines();
}

// ABSTRACT STATEMENT
void OgreScriptLSP::Parser::abstract(MaterialScriptAst *scriptAst) {
    auto abstract = std::make_unique<AbstractAst>();

    nextToken();
    abstract->type = getToken();
    switch (getToken().tk) {
        case material_tk: {
            auto scriptTmp = std::make_unique<MaterialScriptAst>("");
            material(scriptTmp.get());
            abstract->body = std::move(scriptTmp->materials[0]);
            break;
        }
        case technique_tk: {
            auto materialTmp = std::make_unique<MaterialAst>();
            materialTechnique(materialTmp.get());
            registerDeclaration(materialTmp->techniques[0].get(), TECHNIQUE_BLOCK);
            abstract->body = std::move(materialTmp->techniques[0]);
            break;
        }
        case pass_tk: {
            auto techniqueTmp = std::make_unique<TechniqueAst>();
            materialPass(techniqueTmp.get());
            registerDeclaration(techniqueTmp->passes[0].get(), PASS_BLOCK);
            abstract->body = std::move(techniqueTmp->passes[0]);
            techniqueTmp->passes.clear();
            break;
        }
        case texture_unit_tk: {
            auto passTmp = std::make_unique<PassAst>();
            materialTexture(passTmp.get());
            registerDeclaration(passTmp->textures[0].get(), TEXTURE_UNIT_BLOCK);
            abstract->body = std::move(passTmp->textures[0]);
            passTmp->textures.clear();
            break;
        }
        case texture_source_tk: {
            auto textureUnit = std::make_unique<TextureUnitAst>();
            materialTextureSource(textureUnit.get());
            registerDeclaration(textureUnit->textureSources[0].get(), TEXTURE_SOURCE_BLOCK);
            abstract->body = std::move(textureUnit->textureSources[0]);
            textureUnit->textureSources.clear();
            break;
        }
        case rtshader_system_tk: {
            auto shader = std::make_unique<RtShaderAst>();
            materialRtShader(shader.get());
            registerDeclaration(shader.get(), RTSHADER_BLOCK);
            abstract->body = std::move(shader);
            break;
        }
        default:
            break;
    }

    scriptAst->abstracts.push_back(std::move(abstract));
}

// SHARED PARAMS STATEMENT
void OgreScriptLSP::Parser::sharedParams(MaterialScriptAst *scriptAst) {
    auto sharedParam = std::make_unique<SharedParamsAst>();

    // consume shared_params_tk token
    nextToken();

    sharedParam->name = getToken();
    consumeToken(identifier, SHARED_PARAMS_NAME_MISSING);
    declarations[std::make_pair(SHARED_PARAMS_BLOCK, sharedParam->name.literal)] = sharedParam->name;
    consumeEndLines();

    consumeOpenCurlyBracket();

    sharedParamsBody(sharedParam.get());

    consumeCloseCurlyBracket();

    scriptAst->sharedParams.push_back(std::move(sharedParam));
}

void OgreScriptLSP::Parser::sharedParamsBody(OgreScriptLSP::SharedParamsAst *sharedParams) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto param = std::make_unique<ParamProgramAst>();
            paramsLine(param.get());
            sharedParams->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_SHARED_PARAMS_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

// SAMPLER STATEMENT
void OgreScriptLSP::Parser::sampler(MaterialScriptAst *scriptAst) {
    auto sampler = std::make_unique<SamplerAst>();

    // consume sampler_tk token
    nextToken();

    sampler->name = getToken();
    consumeToken(identifier, PROGRAM_NAME_MISSING, true);
    declarations[std::make_pair(SAMPLER_BLOCK, sampler->name.literal)] = sampler->name;

    consumeOpenCurlyBracket();

    samplerBody(sampler.get());

    consumeCloseCurlyBracket();

    scriptAst->sampler.push_back(std::move(sampler));
}

void OgreScriptLSP::Parser::samplerBody(SamplerAst *sampler) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto param = std::make_unique<SamplerParamAst>();
            paramsLine(param.get());
            sampler->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_SAMPLER_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

// PROGRAM STATEMENT
void OgreScriptLSP::Parser::program(MaterialScriptAst *scriptAst) {
    auto program = std::make_unique<ProgramAst>();

    program->type = getToken();
    nextToken();

    program->name = getToken();
    consumeToken(identifier, PROGRAM_NAME_MISSING);
    declarations[std::make_pair(getProgramBlockIdk(program->type.tk), program->name.literal)] = program->name;

    programOpt(program.get());

    consumeOpenCurlyBracket();

    programBody(program.get());

    consumeCloseCurlyBracket();

    scriptAst->programs.push_back(std::move(program));
}

void OgreScriptLSP::Parser::programOpt(OgreScriptLSP::ProgramAst *program) {
    if (getToken().tk != identifier) {
        throw ParseException(PROGRAM_HIGH_LEVEL_MISSING, getToken().toRange());
    }
    while (!isEof() && getToken().tk == identifier && !isMainStructure()) {
        program->highLevelProgramsType.push_back(getToken());
        nextToken();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::programBody(OgreScriptLSP::ProgramAst *program) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == default_params_tk) {
            programDefaults(program);
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<ParamProgramAst>();
            paramsLine(param.get());
            program->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_PROGRAM_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::programDefaults(OgreScriptLSP::ProgramAst *program) {
    nextTokenAndConsumeEndLines();

    consumeOpenCurlyBracket();

    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        auto tk = getToken();
        if (tk.tk == shared_params_ref_tk) {
            auto sharedParam = std::make_unique<MaterialProgramSharedParamAst>();
            programSharedParams(sharedParam.get());
            program->sharedParams.push_back(std::move(sharedParam));
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<ParamProgramDefaultAst>();
            paramsLine(param.get());
            program->defaults.push_back(std::move(param));
            continue;
        }
        exceptions.push_back(ParseException(NOT_VALID_PARAM, tk.toRange()));
        recuperateLine();
    }

    consumeCloseCurlyBracket();
}

// MATERIAL STATEMENT
void OgreScriptLSP::Parser::material(OgreScriptLSP::MaterialScriptAst *scriptAst) {
    auto material = std::make_unique<MaterialAst>();
    material->parent = {EOF_tk, ""};

    // consume material_tk token
    material->symbol = getToken();
    nextToken();

    objectDefinition(material.get(), MATERIAL_NAME_MISSION_ERROR, MATERIAL_INHERIT_ERROR);
    declarations[std::make_pair(MATERIAL_BLOCK, material->name.literal)] = material->name;

    consumeOpenCurlyBracket();

    materialBody(material.get());

    consumeCloseCurlyBracket();

    scriptAst->materials.push_back(std::move(material));
}

void OgreScriptLSP::Parser::materialBody(OgreScriptLSP::MaterialAst *material) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == technique_tk) {
            materialTechnique(material);
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<MaterialParamAst>();
            paramsLine(param.get());
            material->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTechnique(OgreScriptLSP::MaterialAst *material) {
    auto technique = std::make_unique<TechniqueAst>();

    // consume technique_tk token
    nextToken();

    objectDefinition(technique.get(), TECHNIQUE_NAME_MISSION_ERROR, TECHNIQUE_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialTechniqueBody(technique.get());

    consumeCloseCurlyBracket();

    material->techniques.push_back(std::move(technique));
}

void OgreScriptLSP::Parser::materialTechniqueBody(OgreScriptLSP::TechniqueAst *technique) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == pass_tk) {
            materialPass(technique);
            continue;
        }
        if (tk.tk == shadow_receiver_material_tk || tk.tk == shadow_caster_material_tk) {
            techniqueShadowMaterial(technique);
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<PassParamAst>();
            paramsLine(param.get());
            technique->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TECHNIQUE_BODY, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::techniqueShadowMaterial(OgreScriptLSP::TechniqueAst *technique) {
    auto shadowMat = std::make_unique<ShadowMaterialAst>();

    // consume shadow_caster_material_tk|shadow_receiver_material_tk token
    shadowMat->type = getToken();
    nextToken();

    if (getToken().tk != identifier && getToken().tk != string_literal) {
        exceptions.push_back(ParseException(NOT_VALID_SHADOW_MATERIAL_REF, getToken().toRange()));
        recuperateLine();
        return;
    }
    shadowMat->reference = getToken();
    nextToken();

    if (getToken().tk != endl_tk) {
        exceptions.push_back(ParseException(NOT_VALID_SHADOW_MATERIAL_REF, getToken().toRange()));
        recuperateLine();
        return;
    }

    technique->shadowMaterials.push_back(std::move(shadowMat));
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialPass(OgreScriptLSP::TechniqueAst *technique) {
    auto pass = std::make_unique<PassAst>();

    // consume pass_tk token
    nextToken();

    objectDefinition(pass.get(), PASS_NAME_MISSION_ERROR, PASS_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialPassBody(pass.get());

    consumeCloseCurlyBracket();

    technique->passes.push_back(std::move(pass));
}

void OgreScriptLSP::Parser::materialPassBody(OgreScriptLSP::PassAst *pass) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == texture_unit_tk) {
            materialTexture(pass);
            continue;
        }
        if (tk.tk == rtshader_system_tk) {
            auto shader = std::make_unique<RtShaderAst>();
            materialRtShader(shader.get());
            pass->shaders.push_back(std::move(shader));
            continue;
        }
        if (tk.tk == vertex_program_ref_tk || tk.tk == fragment_program_ref_tk || tk.tk == geometry_program_ref_tk ||
            tk.tk == tessellation_hull_program_ref_tk || tk.tk == tessellation_domain_program_ref_tk ||
            tk.tk == compute_program_ref_tk) {
            materialProgramRef(pass);
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<PassParamAst>();
            paramsLine(param.get());
            pass->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PASS_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTexture(OgreScriptLSP::PassAst *pass) {
    auto texture = std::make_unique<TextureUnitAst>();

    // consume texture_unit_tk token
    nextToken();

    objectDefinition(texture.get(), TEXTURE_NAME_MISSION_ERROR, TEXTURE_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialTextureBody(texture.get());

    consumeCloseCurlyBracket();

    pass->textures.push_back(std::move(texture));
}

void OgreScriptLSP::Parser::materialTextureBody(OgreScriptLSP::TextureUnitAst *texture) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == rtshader_system_tk) {
            auto shader = std::make_unique<RtShaderAst>();
            materialRtShader(shader.get());
            texture->shaders.push_back(std::move(shader));
            continue;
        }
        if (tk.tk == texture_source_tk) {
            materialTextureSource(texture);
            continue;
        }
        if (tk.tk == sampler_ref_tk) {
            samplerRef(texture);
            continue;
        }
        if (tk.tk == identifier) {
            auto param = std::make_unique<TextureUnitParamAst>();
            paramsLine(param.get());
            texture->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TEXTURE_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::samplerRef(TextureUnitAst *textureUnit) {
    auto samplerRef = std::make_unique<SamplerRefAst>();

    // consume shadow_caster_material_tk|shadow_receiver_material_tk token
    nextToken();

    if (getToken().tk != identifier && getToken().tk != string_literal) {
        exceptions.push_back(ParseException(NOT_VALID_SAMPLER_REF, getToken().toRange()));
        recuperateLine();
        return;
    }
    samplerRef->identifier = getToken();
    nextToken();

    if (getToken().tk != endl_tk) {
        exceptions.push_back(ParseException(NOT_VALID_SAMPLER_REF, getToken().toRange()));
        recuperateLine();
        return;
    }

    textureUnit->sampleReferences.push_back(std::move(samplerRef));
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTextureSource(OgreScriptLSP::TextureUnitAst *texture) {
    auto textureSource = std::make_unique<TextureSourceAst>();

    // consume texture_source_tk token
    nextToken();

    objectDefinition(textureSource.get(), TEXTURE_SOURCE_MISSING_ERROR, TEXTURE_SOURCE_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialTextureSourceBody(textureSource.get());

    consumeCloseCurlyBracket();
    texture->textureSources.push_back(std::move(textureSource));
}

void OgreScriptLSP::Parser::materialTextureSourceBody(OgreScriptLSP::TextureSourceAst *shader) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto param = std::make_unique<RtShaderParamAst>();
            paramsLine(param.get());
            shader->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TEXTURE_SOURCE_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialRtShader(OgreScriptLSP::RtShaderAst *shader) {
    // consume rtshader_system_tk token
    nextToken();

    objectDefinition(shader, RTSHADER_MISSING_ERROR, RTSHADER_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialRtShaderBody(shader);

    consumeCloseCurlyBracket();
}

void OgreScriptLSP::Parser::materialRtShaderBody(OgreScriptLSP::RtShaderAst *shader) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto param = std::make_unique<RtShaderParamAst>();
            paramsLine(param.get());
            shader->params.push_back(std::move(param));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_RTSHADER_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialProgramRef(OgreScriptLSP::PassAst *pass) {
    auto programRef = std::make_unique<MaterialProgramAst>();

    // consume [program_ref_tk] token
    programRef->type = getToken();
    nextTokenAndConsumeEndLines();

    auto tk = getToken();
    if (tk.tk == identifier) {
        programRef->name = tk;
        nextTokenAndConsumeEndLines();
    } else if (tk.tk != left_curly_bracket_tk) {
        throw ParseException(MATERIAL_PROGRAM_NAME_MISSING_ERROR, tk.toRange());
    }

    consumeOpenCurlyBracket();

    materialProgramRefBody(programRef.get());

    consumeCloseCurlyBracket();

    pass->programsReferences.push_back(std::move(programRef));
}

void OgreScriptLSP::Parser::materialProgramRefBody(OgreScriptLSP::MaterialProgramAst *programRef) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto param = std::make_unique<MaterialProgramParamAst>();
            paramsLine(param.get());
            programRef->params.push_back(std::move(param));
            continue;
        }
        if (tk.tk == shared_params_ref_tk) {
            auto sharedParam = std::make_unique<MaterialProgramSharedParamAst>();
            programSharedParams(sharedParam.get());
            programRef->sharedParams.push_back(std::move(sharedParam));
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PROGRAM_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::programSharedParams(MaterialProgramSharedParamAst *sharedParam) {
    // consume shared_params_ref_tk
    nextToken();

    if (getToken().tk != identifier) {
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PASS_PARAM, getToken().toRange()));
        recuperateLine();
        return;
    }
    sharedParam->identifier = getToken();
    nextToken();

    if (getToken().tk != endl_tk) {
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PASS_PARAM, getToken().toRange()));
        recuperateLine();
        return;
    }

    consumeEndLines();
}

// THIS IS COMMON SECTION
void OgreScriptLSP::Parser::paramsLine(ParamAst *params) {
    while (!isEof() && getToken().tk != endl_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk != identifier && tk.tk != string_literal && tk.tk != number_literal &&
            tk.tk != variable && tk.tk != comma_tk) {
            exceptions.push_back(ParseException(NOT_VALID_PARAM, tk.toRange()));
            recuperateLine();
            return;
        }
        params->items.push_back(tk);
        nextToken();
    }
    nextTokenAndConsumeEndLines();
}

void OgreScriptLSP::Parser::recuperateLine() {
    while (!isEof() && getToken().tk != endl_tk) nextToken();
    consumeEndLines();
}

void OgreScriptLSP::Parser::nextTokenAndConsumeEndLines() {
    nextToken();
    consumeEndLines();
}

void OgreScriptLSP::Parser::consumeEndLines() {
    while (!isEof() && getToken().tk == endl_tk) {
        nextToken();
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Parser::getToken() {
    if (isEof()) {
        return {EOF_tk};
    }
    return tokens[currentToken];
}

void OgreScriptLSP::Parser::objectDefinition(AstObject *astObject, std::string error1, std::string error2,
                                             bool notTopLevelObject) {

    if (notTopLevelObject) {
        if (getToken().tk == left_curly_bracket_tk || getToken().tk == endl_tk) {
            consumeEndLines();
            return;
        } else if (getToken().tk == match_literal) {
            astObject->name = getToken();
            nextTokenAndConsumeEndLines();
            return;
        }
    }

    if (!notTopLevelObject || getToken().tk != colon_tk) {
        if (getToken().tk != identifier && getToken().tk != string_literal && getToken().tk != number_literal) {
            throw ParseException(std::move(error1), getToken().toRange());
        }
        astObject->name = getToken();
        nextToken();
    }

    if (getToken().tk == colon_tk) {
        nextToken();

        if (getToken().tk != identifier && getToken().tk != string_literal) {
            throw ParseException(std::move(error2), getToken().toRange());
        }
        astObject->parent = getToken();
        nextToken();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::consumeOpenCurlyBracket() {
    consumeToken(left_curly_bracket_tk, CURLY_BRACKET_START_MISSING, true);
}

void OgreScriptLSP::Parser::consumeCloseCurlyBracket() {
    consumeToken(right_curly_bracket_tk, CURLY_BRACKET_END_MISSING, true);
}

void OgreScriptLSP::Parser::consumeToken(Token token, std::string errorMessage, bool consumeEndLines) {
    if (getToken().tk != token) {
        throw ParseException(std::move(errorMessage), getToken().toRange());
    }
    if (consumeEndLines) {
        nextTokenAndConsumeEndLines();
    } else {
        nextToken();
    }
}

void OgreScriptLSP::Parser::nextToken() {
    if (!isEof()) {
        currentToken++;
    }
}

bool OgreScriptLSP::Parser::isEof() {
    return currentToken >= tokens.size();
}

bool OgreScriptLSP::Parser::isMainStructure() {
    auto tk = getToken().tk;
    return tk == vertex_program_tk || tk == fragment_program_tk || tk == material_tk;
}

void OgreScriptLSP::Parser::initSwap() {
    currentToken = 0;
}

void OgreScriptLSP::Parser::registerDeclaration(OgreScriptLSP::AstObject *object, int type) {
    declarations[std::make_pair(type, object->name.literal)] = object->name;
}

const OgreScriptLSP::Scanner *OgreScriptLSP::Parser::getScanner() const {
    return scanner.get();
}
