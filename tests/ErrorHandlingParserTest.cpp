//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/parser.h"

TEST (ErrorHandlingParserTest, parse_ShouldFailWithErrorsAnsRecuperate) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/parse/frag_vert_declarations_with_errors.material";
    parser->loadScript(scriptFile);

    parser->parse();
}
