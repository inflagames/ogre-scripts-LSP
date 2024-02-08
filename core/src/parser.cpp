#include "../inc/parser.h"

OgreScriptLSP::Parser::Parser() {
    scanner = new Scanner();
}

OgreScriptLSP::Parser::~Parser() {
    delete scanner;
    delete script;
}

void OgreScriptLSP::Parser::loadScript(const std::string &scriptFile) {
    scanner->loadScript(uriToPath(scriptFile));
    tokens = scanner->parse();
    currentToken = 0;
    script = new MaterialScriptAst(scriptFile);
}

std::string OgreScriptLSP::Parser::uriToPath(const std::string &uri) {
    if (uri.starts_with("file://")) {
        return uri.substr(7);
    }
    return uri;
}

ResultBase *OgreScriptLSP::Parser::goToDefinition(Position position) {
    // toDo (gonzalezext)[03.02.24]: this should be extended to more than one script AST when importing be supported
    // material
    for (auto mat: script->materials) {
        TokenValue parent = mat->parent;
        TokenValue name = mat->name;
        Range rangeParent = {{parent.line, parent.column},
                             {parent.line, parent.column + parent.size - 1}};
        Range rangeName = {{name.line, name.column},
                           {name.line, name.column + name.size - 1}};
        if (name.tk == identifier && rangeName.inRange(position)) {
            // go to definition from same class
            return new Location(script->uri, rangeName);
        }
        if (parent.tk == identifier && rangeParent.inRange(position)) {
            // go to definition from parent
            for (auto mat2: script->materials) {
                if (mat2->name.literal == parent.literal) {
                    TokenValue ftk = mat2->name;
                    Range rangeLocation = {{ftk.line, ftk.column},
                                           {ftk.line, ftk.column + ftk.size - 1}};
                    return new Location(script->uri, rangeLocation);
                }
            }
            return new Location(script->uri, rangeParent);
        }
    }

    // variables todo: support for goto in variables not implemented

    // programs
    for (auto prog: script->programs) {
        Range rangeProg = {{prog->name.line, prog->name.column},
                           {prog->name.line, prog->name.column + prog->name.size - 1}};
        if (rangeProg.inRange(position)) {
            // return same identifier position
            return new Location(script->uri, rangeProg);
        }
    }
    for (auto mat: script->materials) {
        for (auto tech: mat->techniques) {
            for (auto pass: tech->passes) {
                for (auto progRef: pass->programsReferences) {
                    Range rangeProgRef = {{progRef->name.line, progRef->name.column},
                                          {progRef->name.line, progRef->name.column + progRef->name.size - 1}};
                    if (rangeProgRef.inRange(position)) {
                        for (auto prog: script->programs) {
                            if (prog->name.literal == progRef->name.literal) {
                                Range rangeProg = {{prog->name.line, prog->name.column},
                                                   {prog->name.line, prog->name.column + prog->name.size - 1}};
                                return new Location(script->uri, rangeProg);
                            }
                        }

                        // same position in case of not definition found
                        return new Location(script->uri, rangeProgRef);
                    }
                }
            }
        }
    }

    // by default return same position range
    Range range = {{position.line, position.character},
                   {position.line, position.character}};
    return new Location(script->uri, range);
}

void OgreScriptLSP::Parser::parse() {
    currentToken = 0;

    bool recuperate = false;
    while (!isEof()) {
        consumeEndLines();
        TokenValue tk = getToken();
        try {
            if (tk.tk == fragment_program_tk || tk.tk == vertex_program_tk) {
                recuperate = false;
                program(script);
            } else if (tk.tk == material_tk) {
                recuperate = false;
                material(script);
            } else if (!recuperate) {
                exceptions.push_back(ParseException(INVALID_TOKEN, tk.line, tk.column));
                recuperate = true;
            }
        } catch (ParseException e) {
            exceptions.push_back(e);
            recuperate = true;
            continue;
        }
        if (recuperate) {
            nextTokenAndConsumeEndLines();
        }
    }
}

// PROGRAM STATEMENT
void OgreScriptLSP::Parser::program(MaterialScriptAst *scriptAst) {
    auto *program = new ProgramAst();

    if (getToken().tk == fragment_program_tk) {
        program->type = ProgramAst::fragment;
    } else {
        program->type = ProgramAst::vertex;
    }
    nextToken();

    if (getToken().tk != identifier) {
        throw ParseException(PROGRAM_NAME_MISSION, getToken().line, getToken().column);
    }
    program->name = getToken();
    nextToken();

    programOpt(program);

    consumeOpenCurlyBracket();

    programBody(program);

    consumeCloseCurlyBracket();

    scriptAst->programs.push_back(program);
}

void OgreScriptLSP::Parser::programOpt(OgreScriptLSP::ProgramAst *program) {
    if (getToken().tk != identifier) {
        throw ParseException(PROGRAM_HIGH_LEVEL_MISSING, getToken().line, getToken().column);
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
        exceptions.push_back(ParseException(NOT_VALID_PROGRAM_PARAM, tk.line, tk.column));
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
            exceptions.push_back(ParseException(NOT_VALID_PARAM, tk.line, tk.column));
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
    nextToken();

    auto def = objectDefinition(MATERIAL_NAME_MISSION_ERROR, MATERIAL_INHERIT_ERROR);
    material->name = def[0];
    if (def.size() > 1) {
        material->parent = def[1];
    }

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
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PARAM, tk.line, tk.column));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTechnique(OgreScriptLSP::MaterialAst *material) {
    auto *technique = new TechniqueAst();

    // consume technique_tk token
    nextTokenAndConsumeEndLines();

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

        // recuperate line
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TECHNIQUE_BODY, tk.line, tk.column));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialPass(OgreScriptLSP::TechniqueAst *technique) {
    auto *pass = new PassAst();

    // consume pass_tk token
    nextTokenAndConsumeEndLines();

    materialPassName(pass);

    consumeOpenCurlyBracket();

    materialPassBody(pass);

    consumeCloseCurlyBracket();

    technique->passes.push_back(pass);
}

void OgreScriptLSP::Parser::materialPassName(OgreScriptLSP::PassAst *pass) {
    if (getToken().tk == identifier) {
        pass->name = getToken();
        nextToken();

        if (getToken().tk == colon_tk) {
            nextToken();

            auto tk = getToken();
            if (tk.tk != identifier) {
                throw ParseException(MATERIAL_PASS_INHERIT_ERROR, tk.line, tk.column);
            }

            pass->parent = tk;
            nextTokenAndConsumeEndLines();
        }
    } else if (getToken().tk == match_literal) {
        pass->name = getToken();
        nextTokenAndConsumeEndLines();
    } else if (getToken().tk != left_curly_bracket_tk) {
        auto tk = getToken();
        throw ParseException(MATERIAL_PASS_NAME_MISSION_ERROR, tk.line, tk.column);
    }
}

void OgreScriptLSP::Parser::materialPassBody(OgreScriptLSP::PassAst *pass) {
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk == texture_unit_tk) {
            materialTexture(pass);
            continue;
        }
        if (tk.tk == vertex_program_ref_tk || tk.tk == fragment_program_ref_tk) {
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
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PASS_PARAM, tk.line, tk.column));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialTexture(OgreScriptLSP::PassAst *pass) {
    auto *texture = new TextureUnitAst();

    // consume texture_unit_tk token
    nextTokenAndConsumeEndLines();

    auto tk = getToken();
    if (tk.tk == identifier) {
        texture->name = tk;
        nextTokenAndConsumeEndLines();
    } else if (tk.tk != left_curly_bracket_tk) {
        throw ParseException(MATERIAL_TEXTURE_NAME_MISSION_ERROR, tk.line, tk.column);
    }

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
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_TEXTURE_PARAM, tk.line, tk.column));
        recuperateLine();
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::materialProgramRef(OgreScriptLSP::PassAst *pass) {
    auto *programRef = new MaterialProgramAst();

    // consume [fragment_program_tk|vertex_program_tk] token
    programRef->type = getToken();
    nextTokenAndConsumeEndLines();

    auto tk = getToken();
    if (tk.tk == identifier) {
        programRef->name = tk;
        nextTokenAndConsumeEndLines();
    } else if (tk.tk != left_curly_bracket_tk) {
        throw ParseException(MATERIAL_PROGRAM_NAME_MISSION_ERROR, tk.line, tk.column);
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
        exceptions.push_back(ParseException(NOT_VALID_MATERIAL_PROGRAM_PARAM, tk.line, tk.column));
        recuperateLine();
    }
    consumeEndLines();
}

// THIS IS COMMON SECTION
void OgreScriptLSP::Parser::paramsLine(ParamAst *params) {
    while (!isEof() && getToken().tk != endl_tk && !isMainStructure()) {
        TokenValue tk = getToken();
        if (tk.tk != identifier && tk.tk != string_literal && tk.tk != number_literal && tk.tk != variable) {
            exceptions.push_back(ParseException(NOT_VALID_PARAM, tk.line, tk.column));
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
    return tokens[currentToken];
}

std::vector<OgreScriptLSP::TokenValue>
OgreScriptLSP::Parser::objectDefinition(std::string error1, std::string error2) {
    std::vector<TokenValue> res;

    if (getToken().tk != identifier) {
        throw ParseException(std::move(error1), getToken().line, getToken().column);
    }
    res.push_back(getToken());
    nextToken();

    if (getToken().tk == colon_tk) {
        nextToken();

        auto tk = getToken();
        if (tk.tk != identifier) {
            throw ParseException(std::move(error2), tk.line, tk.column);
        }
        res.push_back(getToken());
        nextToken();
    }
    consumeEndLines();
    return res;
}

void OgreScriptLSP::Parser::consumeOpenCurlyBracket() {
    if (getToken().tk != left_curly_bracket_tk) {
        throw ParseException(CURLY_BRACKET_START_MISSING, getToken().line, getToken().column);
    }
    nextTokenAndConsumeEndLines();
}

void OgreScriptLSP::Parser::consumeCloseCurlyBracket() {
    if (getToken().tk != right_curly_bracket_tk) {
        throw ParseException(CURLY_BRACKET_END_MISSING, getToken().line, getToken().column);
    }
    nextTokenAndConsumeEndLines();
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
