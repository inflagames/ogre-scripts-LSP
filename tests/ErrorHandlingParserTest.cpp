//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/parser.h"

TEST (ErrorHandlingParserTest, parse_ShouldFailWithErrorsAnsRecuperate) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/parser/frag_vert_declarations_with_errors.material";
    parser->loadScript(scriptFile);

    parser->parse();
    ASSERT_GT(parser->getExceptions().size(), 0);

    auto exceptions = parser->getExceptions();
    auto e = exceptions[0];
    ASSERT_EQ(PROGRAM_NAME_MISSION, e.message);
    ASSERT_EQ(2, e.line);
    ASSERT_EQ(18, e.column);

    e = exceptions[1];
    ASSERT_EQ(CURLY_BRACKET_START_MISSING, e.message);
    ASSERT_EQ(8, e.line);
    ASSERT_EQ(5, e.column);

    e = exceptions[2];
    ASSERT_EQ(CURLY_BRACKET_END_MISSING, e.message);
    ASSERT_EQ(16, e.line);
    ASSERT_EQ(1, e.column);

    e = exceptions[3];
    ASSERT_EQ(PROGRAM_HIGH_LEVEL_MISSING, e.message);
    ASSERT_EQ(16, e.line);
    ASSERT_EQ(28, e.column);

    e = exceptions[4];
    ASSERT_EQ(NOT_VALID_PARAM, e.message);
    ASSERT_EQ(27, e.line);
    ASSERT_EQ(3, e.column);

    e = exceptions[5];
    ASSERT_EQ(NOT_VALID_PARAM, e.message);
    ASSERT_EQ(29, e.line);
    ASSERT_EQ(15, e.column);
}
