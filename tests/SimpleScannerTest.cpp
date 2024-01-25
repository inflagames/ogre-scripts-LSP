//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/scanner.h"

TEST (SimpleScannerTest, scanner_ShouldOpenScriptFile) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    std::string scriptFile = "./examples/frag_vert_declarations.material";
    scanner->loadScript(scriptFile);

    ASSERT_TRUE(scanner->file.is_open());
}

TEST (SimpleScannerTest, scanner_ShouldReadCorrectlyTheTokens) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/frag_vert_declarations.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::fragment_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::glsl_tk,
            OgreScriptLSP::glsles_tk, OgreScriptLSP::left_curly_bracket_tk,
            // line
            OgreScriptLSP::source_tk, OgreScriptLSP::identifier,
            // line
            OgreScriptLSP::right_curly_bracket_tk,
            // line
            OgreScriptLSP::vertex_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::glsl_tk,
            OgreScriptLSP::glsles_tk, OgreScriptLSP::left_curly_bracket_tk,
            // line
            OgreScriptLSP::source_tk, OgreScriptLSP::identifier,
            // line
            OgreScriptLSP::right_curly_bracket_tk,
    };

    std::vector<OgreScriptLSP::TokenValue> result = scanner->parse();
    ASSERT_EQ(tokens.size(), result.size());
    for (int i = 0; i < tokens.size(); ++i) {
        ASSERT_EQ(tokens[i], result[i].tk);
    }
}
