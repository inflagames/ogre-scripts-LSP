//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/scanner.h"

TEST (SimpleScannerTest, scanner_ShouldOpenScriptFile) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    std::string scriptFile = "./examples/scanner/frag_vert_declarations.material";
    scanner->loadScript(scriptFile);

    ASSERT_TRUE(scanner->file.is_open());
}

TEST (SimpleScannerTest, scanner_ShouldReadCorrectlyTheTokensFromFragments) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/frag_vert_declarations.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::fragment_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::identifier, OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::source_tk, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::vertex_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::source_tk, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::default_params_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::number_literal, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk
    };

    std::vector<OgreScriptLSP::TokenValue> result = scanner->parse();
    ASSERT_EQ(tokens.size(), result.size());
    for (int i = 0; i < tokens.size(); ++i) {
        ASSERT_EQ(tokens[i], result[i].tk);
    }
}

TEST (SimpleScannerTest, scanner_ShouldReadCorrectlyTheTokensFromMaterial) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/simple_material.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::abstract_tk, OgreScriptLSP::pass_tk, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::variable, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::material_tk, OgreScriptLSP::identifier, OgreScriptLSP::left_curly_bracket_tk,
            OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::technique_tk, OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::pass_tk, OgreScriptLSP::colon_tk, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::set_tk, OgreScriptLSP::variable, OgreScriptLSP::string_literal, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
    };

    std::vector<OgreScriptLSP::TokenValue> result = scanner->parse();
    std::vector<OgreScriptLSP::Token> tokens2;
    for (int i = 0; i < result.size(); ++i) {
        tokens2.push_back(result[i].tk);
    }
    ASSERT_EQ(tokens.size(), result.size());
    for (int i = 0; i < tokens.size(); ++i) {
        ASSERT_EQ(tokens[i], result[i].tk);
    }
}

TEST (SimpleScannerTest, scanner_ShouldReadCorrectlyTheTokensFromPass) {
    OgreScriptLSP::Scanner *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/matching_names.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::abstract_tk, OgreScriptLSP::technique_tk, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::pass_tk, OgreScriptLSP::match_literal, OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
    };

    std::vector<OgreScriptLSP::TokenValue> result = scanner->parse();
    ASSERT_EQ(tokens.size(), result.size());
    for (int i = 0; i < tokens.size(); ++i) {
        ASSERT_EQ(tokens[i], result[i].tk);
    }
}
