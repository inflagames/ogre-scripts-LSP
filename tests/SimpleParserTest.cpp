//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/parser.h"

TEST (SimpleParserTest, scanner_ShouldOpenScriptFile) {
    OgreScriptLSP::Parser *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/scanner/frag_vert_declarations.material";
    parser->loadScript(scriptFile);

    parser->parse();

    OgreScriptLSP::MaterialScriptAst *s = parser->getScript();
    ASSERT_TRUE(s != nullptr);
    ASSERT_EQ(2, s->programs.size());

    OgreScriptLSP::ProgramAst *p = s->programs[0];
    ASSERT_EQ(2, p->highLevelProgramsType.size());
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(0, p->defaults.size());

    p = s->programs[1];
    ASSERT_EQ(1, p->highLevelProgramsType.size());
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(1, p->defaults.size());
}
