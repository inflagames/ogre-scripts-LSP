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

    while (currentToken < tokens.size()) {
        consumeEndLines();
        TokenValue tk = getToken();
        if (tk.tk == fragment_program_tk || tk.tk == vertex_program_tk) {
            program(script);
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
        // toDo (gonzalezext)[28.01.24]: throw exception
    }
    program->name = getToken();
    nextToken();

    programOpt(program);

    if (getToken().tk != left_curly_bracket_tk) {
        // toDo (gonzalezext)[28.01.24]: throw exception
    }
    nextTokenAndConsumeEndLines();

    programBody(program);

    if (getToken().tk != right_curly_bracket_tk) {
        // toDo (gonzalezext)[28.01.24]: throw exception
    }
    nextTokenAndConsumeEndLines();

    script->programs.push_back(program);
}

void OgreScriptLSP::Parser::programOpt(OgreScriptLSP::ProgramAst *program) {
    if (getToken().tk != identifier) {
        // toDo (gonzalezext)[28.01.24]: throw exception
    }
    while (getToken().tk == identifier) {
        program->highLevelProgramsType.push_back(getToken());
        nextToken();
    }
    if (getToken().tk == endl_tk) {
        nextTokenAndConsumeEndLines();
    }
}

void OgreScriptLSP::Parser::programBody(OgreScriptLSP::ProgramAst *program) {
    while (getToken().tk != right_curly_bracket_tk) {
        TokenValue tk = getToken();
        if (tk.tk == default_params_tk) {
            programDefaults(program);
            continue;
        }
        // toDo (gonzalezext)[28.01.24]: recuperate if error
        auto *param = new ParamProgramAst();
        paramsLine(param);
        program->params.push_back(param);
    }
    consumeEndLines();
}

void OgreScriptLSP::Parser::programDefaults(OgreScriptLSP::ProgramAst *program) {
    nextTokenAndConsumeEndLines();

    if (getToken().tk != left_curly_bracket_tk) {
        // toDo (gonzalezext)[28.01.24]: throw exception
    }
    nextTokenAndConsumeEndLines();

    while (getToken().tk != right_curly_bracket_tk) {
        // toDo (gonzalezext)[28.01.24]: recuperate if error
        auto *param = new ParamProgramDefaultAst();
        paramsLine(param);
        program->defaults.push_back(param);
    }
    nextTokenAndConsumeEndLines();
}

void OgreScriptLSP::Parser::paramsLine(ParamAst *params) {
    while (getToken().tk != endl_tk) {
        TokenValue tk = getToken();
        if (tk.tk != identifier && tk.tk != string_literal && tk.tk != number_literal && tk.tk != variable) {
            // toDo (gonzalezext)[28.01.24]: throw exception
        }
        params->items.push_back(tk);
        nextToken();
    }
    nextTokenAndConsumeEndLines();
}

OgreScriptLSP::TokenValue OgreScriptLSP::Parser::getToken() {
    return tokens[currentToken];
}

void OgreScriptLSP::Parser::nextTokenAndConsumeEndLines() {
    nextToken();
    consumeEndLines();
}

void OgreScriptLSP::Parser::consumeEndLines() {
    while (getToken().tk == endl_tk) {
        nextToken();
    }
}

void OgreScriptLSP::Parser::nextToken() {
    currentToken++;
    if (currentToken >= tokens.size()) {
        // toDo (gonzalezext)[28.01.24]: throw exception when finish
    }
}
