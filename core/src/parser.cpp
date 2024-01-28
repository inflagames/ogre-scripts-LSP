//
// Created by guille on 21.01.24.
//

#include "../inc/parser.h"

OgreScriptLSP::Parser::Parser() {
    scanner = new Scanner();
}

void OgreScriptLSP::Parser::loadScript(const std::string &scriptFile) {
    scanner->loadScript(scriptFile);
    tokens = scanner->parse();
    currentToken = 0;
    script = nullptr;
}

void OgreScriptLSP::Parser::parse() {
    script = new MaterialScriptAst();

    bool recuperate = false;
    while (!isEof()) {
        consumeEndLines();
        TokenValue tk = getToken();
        try {
            if (tk.tk == fragment_program_tk || tk.tk == vertex_program_tk) {
                recuperate = false;
                program(script);
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

    if (getToken().tk != left_curly_bracket_tk) {
        throw ParseException(CURLY_BRACKET_START_MISSING, getToken().line, getToken().column);
    }
    nextTokenAndConsumeEndLines();

    programBody(program);

    if (getToken().tk != right_curly_bracket_tk) {
        throw ParseException(CURLY_BRACKET_END_MISSING, getToken().line, getToken().column);
    }
    nextTokenAndConsumeEndLines();

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
    while (!isEof() && getToken().tk != right_curly_bracket_tk && !isMainStructure() ) {
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

    if (getToken().tk != left_curly_bracket_tk) {
        throw ParseException(CURLY_BRACKET_START_MISSING, getToken().line, getToken().column);
    }
    nextTokenAndConsumeEndLines();

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
    nextTokenAndConsumeEndLines();
}

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
