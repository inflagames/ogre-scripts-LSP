//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/scanner.h"

TEST (SimpleScannerTest, scanner_ShouldOpenScriptFile) {
    Jsai::Scanner *scanner = new Jsai::Scanner();
    std::string scriptFile = "./examples/add_two_numbers.js";
    scanner->loadScript(scriptFile);

    ASSERT_TRUE(scanner->file.is_open());
}

TEST (SimpleScannerTest, scanner_ShouldReadCorrectlyTheTokens) {
    Jsai::Scanner *scanner = new Jsai::Scanner();
    scanner->loadScript("./examples/add_two_numbers.js");

    std::vector<Jsai::Token> tokens = {
            // line
            Jsai::Token::const_tk, Jsai::Token::identifier, Jsai::Token::equals_tk, Jsai::Token::number_literal,
            Jsai::Token::semicolon_tk,
            // line
            Jsai::Token::let_tk, Jsai::Token::identifier, Jsai::Token::equals_tk, Jsai::Token::number_literal,
            Jsai::Token::semicolon_tk,
            // line
            Jsai::Token::var_tk, Jsai::Token::identifier, Jsai::Token::equals_tk, Jsai::Token::identifier,
            Jsai::Token::plus_tk, Jsai::Token::identifier, Jsai::Token::semicolon_tk,
            // line
            Jsai::Token::identifier, Jsai::Token::period_tk, Jsai::Token::identifier, Jsai::Token::left_parenthesis_tk,
            Jsai::Token::string_literal, Jsai::Token::plus_tk, Jsai::Token::identifier, Jsai::Token::plus_tk,
            Jsai::Token::string_literal, Jsai::Token::plus_tk, Jsai::Token::identifier, Jsai::Token::plus_tk,
            Jsai::Token::string_literal, Jsai::Token::plus_tk, Jsai::Token::identifier,
            Jsai::Token::right_parenthesis_tk,
            Jsai::Token::semicolon_tk
    };

    std::vector<Jsai::TokenValue> result = scanner->parse();
    ASSERT_EQ(tokens.size(), result.size());
    for (int i = 0; i < tokens.size(); ++i) {
        ASSERT_EQ(tokens[i], result[i].tk);
    }
}
