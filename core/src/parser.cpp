//
// Created by guille on 21.01.24.
//

#include "../inc/parser.h"

Jsai::Parser::Parser() {
    scanner = new Scanner();
}

void Jsai::Parser::loadScript(const std::string &scriptFile) {
    scanner->loadScript(scriptFile);
}

void Jsai::Parser::lexer() {

}
