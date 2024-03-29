#include "gtest/gtest.h"

#include "../src/lang/scanner.h"
#include "utils/utils.h"

TEST (ScannerTest, scanner_ShouldOpenScriptFile) {
    auto *scanner = new OgreScriptLSP::Scanner();
    std::string scriptFile = "./examples/scanner/frag_vert_declarations.material";
    scanner->loadScript(scriptFile);

    auto *file = dynamic_cast<std::ifstream *>(scanner->codeStream.get());
    ASSERT_TRUE(file->is_open());
}

TEST (ScannerTest, scanner_ShouldOpenScriptFileAndLoadTokens) {
    auto *scanner = new OgreScriptLSP::Scanner();
    std::string scriptFile = "./examples/full_test_material.material";
    scanner->loadScript(scriptFile);

    auto *file = dynamic_cast<std::ifstream *>(scanner->codeStream.get());
    ASSERT_TRUE(file->is_open());

    scanner->parse();
}

TEST (ScannerTest, scanner_ShouldReadCorrectlyTheListFromProdIssueWithMatchLiteralException) {
    auto *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/bad_tokens.material");

    std::vector<OgreScriptLSP::TokenValue> result = scanner->parse();
    ASSERT_EQ(7, result.size());
    ASSERT_EQ(1, scanner->exceptions.size());
}

TEST (ScannerTest, scanner_ShouldReadCorrectlyTheListOfTokens) {
    auto *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/simple_token_validation.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line 1
            OgreScriptLSP::endl_tk,
            // line 2
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::endl_tk,
            // line 3
            OgreScriptLSP::endl_tk,
            // line 4
            OgreScriptLSP::endl_tk,
            // line 5
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::number_literal, OgreScriptLSP::number_literal, OgreScriptLSP::number_literal,
            OgreScriptLSP::endl_tk,
            // line 6
            OgreScriptLSP::endl_tk,
            // line 7
            OgreScriptLSP::endl_tk,
            // line 8
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
    };
    std::vector<std::string> numbers = {
            ".34", "345.34", "234", "0.34", ".34", "-345.34", "-234", "-0.34", "-.34",
            ".34f", "345.34f", "234f", "0.34f", ".34f", "-345.34f", "-234f", "-0.34f", "-.34f"
    };
    std::vector<std::string> identifiers = {
            "10points.png", "PF_BYTE_RGBA", "asdf&asdfv", "adsf234/asdf/asdf", "asdf.asdf", "1234.0f34df"
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

    // validate number literals
    for (int i = 0, j = 0; i < tokens.size(); ++i) {
        if (result[i].tk == OgreScriptLSP::number_literal) {
            ASSERT_EQ(numbers[j++], result[i].literal);
        }
    }

    // validate identifiers
    for (int i = 0, j = 0; i < tokens.size(); ++i) {
        if (result[i].tk == OgreScriptLSP::identifier) {
            ASSERT_EQ(identifiers[j++], result[i].literal);
        }
    }
}

TEST (ScannerTest, scannerStringCode_ShouldReadCorrectlyTheTokensFromFragments) {
    auto *scanner = new OgreScriptLSP::Scanner();
    std::string codeStr = test_utils::readFile("./examples/scanner/frag_vert_declarations.material");
    scanner->loadScript("not-needed", codeStr);

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::fragment_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::identifier, OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::vertex_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
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

TEST (ScannerTest, scanner_ShouldReadCorrectlyTheTokensFromFragments) {
    auto *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/frag_vert_declarations.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::fragment_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::identifier, OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::vertex_program_tk, OgreScriptLSP::identifier, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::identifier, OgreScriptLSP::identifier, OgreScriptLSP::endl_tk,
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

TEST (ScannerTest, scanner_ShouldReadCorrectlyTheTokensFromMaterial) {
    auto *scanner = new OgreScriptLSP::Scanner();
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
            OgreScriptLSP::identifier, OgreScriptLSP::variable, OgreScriptLSP::string_literal, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::right_curly_bracket_tk, OgreScriptLSP::endl_tk,
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

TEST (ScannerTest, scanner_ShouldReadCorrectlyTheTokensFromPass) {
    auto *scanner = new OgreScriptLSP::Scanner();
    scanner->loadScript("./examples/scanner/matching_names.material");

    std::vector<OgreScriptLSP::Token> tokens = {
            // line
            OgreScriptLSP::abstract_tk, OgreScriptLSP::technique_tk, OgreScriptLSP::identifier,
            OgreScriptLSP::left_curly_bracket_tk, OgreScriptLSP::endl_tk,
            // line
            OgreScriptLSP::pass_tk, OgreScriptLSP::match_literal, OgreScriptLSP::left_curly_bracket_tk,
            OgreScriptLSP::endl_tk,
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
