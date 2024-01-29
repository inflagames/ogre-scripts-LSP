//
// Created by guille on 21.01.24.
//

#include <utility>

#include "../inc/parser.h"

OgreScriptLSP::Parser::Parser() {
    scanner = new Scanner();
}

void OgreScriptLSP::Parser::loadScript(const std::string &scriptFile) {
    scanner->loadScript(uriToPath(scriptFile));
    tokens = scanner->parse();
    currentToken = 0;
    script = nullptr;
}

std::string OgreScriptLSP::Parser::uriToPath(const std::string &uri) {
    if (uri.starts_with("file://")) {
        return uri.substr(7);
    }
    return uri;
}

void OgreScriptLSP::Parser::parse() {
    currentToken = 0;
    script = new MaterialScriptAst();

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

ResultArray OgreScriptLSP::Parser::formatting(Range range) {
    ResultArray res;

    // initial values
    currentToken = 0;
    int level = 0;

    // line
    int previousTokenPosition = 0;
    bool firstInLine = true;
    while (!isEof()) {
        auto tk = getToken();

        if (tk.tk == endl_tk) {
            previousTokenPosition = 0;
            firstInLine = true;
            nextToken();
            continue;
        }

        if (tk.tk == left_curly_bracket_tk) {
            level++;
            previousTokenPosition = tk.column + tk.size;
            nextToken();
            continue;
        }
        if (tk.tk == right_curly_bracket_tk) {
            level--;
            previousTokenPosition = tk.column + tk.size;
            nextToken();
            continue;
        }

        int position;

        if (firstInLine) {
            // toDo (gonzalezext)[28.01.24]: update 2 for the corresponding number of spaces
            position = level * 2;
            firstInLine = false;
        } else {
            // toDo (gonzalezext)[28.01.24]: calculate separation base on the tokens
            position = previousTokenPosition + 2;
        }

        if (tk.column > position) {
            res.elements.push_back(new TextEdit({tk.line, previousTokenPosition + 1},
                                                {tk.line, tk.column - position - 1},
                                                ""));
        } else if (tk.column < position) {
            std::string nexText = "";
            for (int i = 0; i < position - tk.column; i++) {
                nexText.push_back(' ');
            }
            res.elements.push_back(new TextEdit({tk.line, previousTokenPosition + 1},
                                                {tk.line, previousTokenPosition + 1},
                                                nexText));
        }

        previousTokenPosition = tk.column + tk.size;
        nextToken();
    }

    for (auto it = res.elements.begin(); it != res.elements.end(); it++) {
        auto e = (TextEdit *) *it;
        if (!range.inRange(e->range)) {
            res.elements.erase(it);
        }
    }

    return res;
}

// PROGRAM STATEMENT
void OgreScriptLSP::Parser::program(MaterialScriptAst *script) {
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

    script->programs.push_back(program);
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
void OgreScriptLSP::Parser::material(OgreScriptLSP::MaterialScriptAst *script) {
    auto *material = new MaterialAst();

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

    script->materials.push_back(material);
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
    nextToken();

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
    nextToken();

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
