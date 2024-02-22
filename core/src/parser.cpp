#include "../inc/parser.h"

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
            } else if (tk.tk == import_tk) {
                recuperate = false;
                importBlock(script.get());
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
    auto *importAst = new ImportAst();

    consumeToken(import_tk, "");

    importAst->imported = getToken();
    if (getToken().tk == asterisk_tk) {
        nextToken();
    } else {
        consumeToken(identifier, NOT_VALID_IMPORT);
    }

    consumeToken(from_tk, NOT_VALID_IMPORT);

    importAst->file = getToken();
    consumeToken(string_literal, NOT_VALID_IMPORT, true);

    scriptAst->imports.push_back(importAst);
}

// ABSTRACT STATEMENT
void OgreScriptLSP::Parser::abstract(MaterialScriptAst *scriptAst) {
    auto *abstract = new AbstractAst();

    nextToken();
    abstract->type = getToken();
    switch (getToken().tk) {
        case material_tk: {
            auto scriptTmp = std::make_unique<MaterialScriptAst>("");
            material(scriptTmp.get());
            abstract->body = scriptTmp->materials[0];
            scriptTmp->materials.clear();
            break;
        }
        case technique_tk: {
            auto materialTmp = std::make_unique<MaterialAst>();
            materialTechnique(materialTmp.get());
            registerDeclaration(materialTmp->techniques[0], TECHNIQUE_BLOCK);
            abstract->body = materialTmp->techniques[0];
            materialTmp->techniques.clear();
            break;
        }
        case pass_tk: {
            auto techniqueTmp = std::make_unique<TechniqueAst>();
            materialPass(techniqueTmp.get());
            registerDeclaration(techniqueTmp->passes[0], PASS_BLOCK);
            abstract->body = techniqueTmp->passes[0];
            techniqueTmp->passes.clear();
            break;
        }
        case texture_unit_tk: {
            auto passTmp = std::make_unique<PassAst>();
            materialTexture(passTmp.get());
            registerDeclaration(passTmp->textures[0], TEXTURE_UNIT_BLOCK);
            abstract->body = passTmp->textures[0];
            passTmp->textures.clear();
            break;
        }
        case rtshader_system_tk: {
            auto passTmp = std::make_unique<PassAst>();
            materialRtShader(passTmp.get());
            registerDeclaration(passTmp->shaders[0], RTSHADER_BLOCK);
            abstract->body = passTmp->shaders[0];
            passTmp->shaders.clear();
            break;
        }
        default:
            break;
    }

    scriptAst->abstracts.push_back(abstract);
}

// PROGRAM STATEMENT
void OgreScriptLSP::Parser::program(MaterialScriptAst *scriptAst) {
    auto *program = new ProgramAst();

    program->type = getToken();
    nextToken();

    program->name = getToken();
    consumeToken(identifier, PROGRAM_NAME_MISSING);
    declarations[std::make_pair(getProgramBlockIdk(program->type.tk), program->name.literal)] = program->name;

    programOpt(program);

    consumeOpenCurlyBracket();

    programBody(program);

    consumeCloseCurlyBracket();

    scriptAst->programs.push_back(program);
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
            auto *param = new ParamProgramAst();
            paramsLine(param);
            program->params.push_back(param);
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
        if (tk.tk != identifier) {
            exceptions.push_back(ParseException(NOT_VALID_PARAM, tk.toRange()));
            recuperateLine();
            continue;
        }
        auto *param = new ParamProgramDefaultAst();
        paramsLine(param);
        program->defaults.push_back(param);
    }

    consumeCloseCurlyBracket();
}

// MATERIAL STATEMENT
void OgreScriptLSP::Parser::material(OgreScriptLSP::MaterialScriptAst *scriptAst) {
    auto *material = new MaterialAst();
    material->parent = {EOF_tk, ""};

    // consume material_tk token
    material->symbol = getToken();
    nextToken();

    objectDefinition(material, MATERIAL_NAME_MISSION_ERROR, MATERIAL_INHERIT_ERROR);
    declarations[std::make_pair(MATERIAL_BLOCK, material->name.literal)] = material->name;

    consumeOpenCurlyBracket();

    materialBody(material);

    consumeCloseCurlyBracket();

    scriptAst->materials.push_back(material);
}

void OgreScriptLSP::Parser::materialBody(OgreScriptLSP::MaterialAst *material) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == technique_tk) {
            materialTechnique(material);
            continue;
        }
        if (tk.tk == identifier) {
            auto *param = new MaterialParamAst();
            paramsLine(param);
            material->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTechnique(OgreScriptLSP::MaterialAst *material) {
    auto *technique = new TechniqueAst();

    // consume technique_tk token
    nextToken();

    objectDefinition(technique, TECHNIQUE_NAME_MISSION_ERROR, TECHNIQUE_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialTechniqueBody(technique);

    consumeCloseCurlyBracket();

    material->techniques.push_back(technique);
}

void OgreScriptLSP::Parser::materialTechniqueBody(OgreScriptLSP::TechniqueAst *technique) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == pass_tk) {
            materialPass(technique);
            continue;
        }
        if (tk.tk == identifier) {
            auto *param = new PassParamAst();
            paramsLine(param);
            technique->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TECHNIQUE_BODY, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialPass(OgreScriptLSP::TechniqueAst *technique) {
    auto *pass = new PassAst();

    // consume pass_tk token
    nextToken();

    objectDefinition(pass, PASS_NAME_MISSION_ERROR, PASS_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialPassBody(pass);

    consumeCloseCurlyBracket();

    technique->passes.push_back(pass);
}

void OgreScriptLSP::Parser::materialPassBody(OgreScriptLSP::PassAst *pass) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == texture_unit_tk) {
            materialTexture(pass);
            continue;
        }
        if (tk.tk == rtshader_system_tk) {
            materialRtShader(pass);
            continue;
        }
        if (tk.tk == vertex_program_ref_tk || tk.tk == fragment_program_ref_tk || tk.tk == geometry_program_ref_tk ||
            tk.tk == tessellation_hull_program_ref_tk || tk.tk == tessellation_domain_program_ref_tk ||
            tk.tk ==  compute_program_ref_tk) {
            materialProgramRef(pass);
            continue;
        }
        if (tk.tk == identifier) {
            auto *param = new PassParamAst();
            paramsLine(param);
            pass->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PASS_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTexture(OgreScriptLSP::PassAst *pass) {
    auto *texture = new TextureUnitAst();

    // consume texture_unit_tk token
    nextToken();

    objectDefinition(texture, TEXTURE_NAME_MISSION_ERROR, TEXTURE_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialTextureBody(texture);

    consumeCloseCurlyBracket();

    pass->textures.push_back(texture);
}

void OgreScriptLSP::Parser::materialTextureBody(OgreScriptLSP::TextureUnitAst *texture) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto *param = new TextureUnitParamAst();
            paramsLine(param);
            texture->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TEXTURE_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialRtShader(OgreScriptLSP::PassAst *pass) {
    auto *shader = new RtShaderAst();

    // consume rtshader_system_tk token
    nextToken();

    objectDefinition(shader, RTSHADER_MISSING_ERROR, RTSHADER_INHERIT_ERROR, true);

    consumeOpenCurlyBracket();

    materialRtShaderBody(shader);

    consumeCloseCurlyBracket();

    pass->shaders.push_back(shader);
}

void OgreScriptLSP::Parser::materialRtShaderBody(OgreScriptLSP::RtShaderAst *shader) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto *param = new RtShaderParamAst();
            paramsLine(param);
            shader->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_RTSHADER_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialProgramRef(OgreScriptLSP::PassAst *pass) {
    auto *programRef = new MaterialProgramAst();

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

    materialProgramRefBody(programRef);

    consumeCloseCurlyBracket();

    pass->programsReferences.push_back(programRef);
}

void OgreScriptLSP::Parser::materialProgramRefBody(OgreScriptLSP::MaterialProgramAst *programRef) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == identifier) {
            auto *param = new MaterialProgramParamAst();
            paramsLine(param);
            programRef->params.push_back(param);
            continue;
        }

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PROGRAM_PARAM, tk.toRange()));
        recuperateLine();
    }
    consumeEndLines();
}

// THIS IS COMMON SECTION
void OgreScriptLSP::Parser::paramsLine(ParamAst *params) {
    while (!isEof() && getToken().tk != endl_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk != identifier && tk.tk != string_literal && tk.tk != number_literal && tk.tk != variable) {
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
        if (getToken().tk != identifier && getToken().tk != string_literal) {
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
