//
// Created by guille on 21.01.24.
//

#include "../inc/parser.h"

OgreScriptLSP::Parser::Parser() {
    scanner = new Scanner();
}

void OgreScriptLSP::Parser::loadScript(const std::string &scriptFile) {
    scanner->loadScript(scriptFile);
}

void OgreScriptLSP::Parser::parse() {

}
