#include "gtest/gtest.h"

#include "../src/parser.h"

TEST (ParserTest, parse_ShouldFailWithErrorsAndRecuperate) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/parser/frag_vert_declarations_with_errors.material";
    parser->loadScript(scriptFile);

    parser->parse();
    ASSERT_GT(parser->getExceptions().size(), 0);

    auto exceptions = parser->getExceptions();
    auto e = exceptions[0];
    ASSERT_EQ(PROGRAM_NAME_MISSING, e.message);
    ASSERT_EQ(1, e.range.start.line);
    ASSERT_EQ(17, e.range.start.character);

    e = exceptions[1];
    ASSERT_EQ(CURLY_BRACKET_START_MISSING, e.message);
    ASSERT_EQ(7, e.range.start.line);
    ASSERT_EQ(4, e.range.start.character);

    e = exceptions[2];
    ASSERT_EQ(CURLY_BRACKET_END_MISSING, e.message);
    ASSERT_EQ(15, e.range.start.line);
    ASSERT_EQ(0, e.range.start.character);

    e = exceptions[3];
    ASSERT_EQ(PROGRAM_HIGH_LEVEL_MISSING, e.message);
    ASSERT_EQ(15, e.range.start.line);
    ASSERT_EQ(27, e.range.start.character);

    e = exceptions[4];
    ASSERT_EQ(NOT_VALID_PARAM, e.message);
    ASSERT_EQ(26, e.range.start.line);
    ASSERT_EQ(2, e.range.start.character);

    e = exceptions[5];
    ASSERT_EQ(NOT_VALID_PARAM, e.message);
    ASSERT_EQ(28, e.range.start.line);
    ASSERT_EQ(14, e.range.start.character);
}
