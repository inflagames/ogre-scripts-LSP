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
    ASSERT_GT(parser->getExceptions().size(), 0);

    auto exceptions = parser->getExceptions();
    auto e = exceptions[0];
    ASSERT_EQ(PROGRAM_NAME_MISSION, e.message);
    ASSERT_EQ(1, e.line);
    ASSERT_EQ(18, e.column);

    e = exceptions[1];
    ASSERT_EQ(CURLY_BRACKET_MISSING, e.message);
    ASSERT_EQ(6, e.line);
    ASSERT_EQ(5, e.column);
}
