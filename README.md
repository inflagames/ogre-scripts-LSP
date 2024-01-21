# JS AI

This project is the base library for interpret the AI scripts for the npcs in the rublem game. 

## ToDo list

- [ ] scanner
- [ ] parser

## Lexer

```
statement = <var_declaration> | <statement_block>
statement_block = <var_declaration> | <left_curly_bracket_tk><statement><right_curly_bracket_tk>

var_declaration = <const_tk><var_list> | <let_tk><var_list> | <var_tk><var_list> 
var_list = <identifier_statement><semicolon_tk> | <identifier_statement><comma_tk><var_list>
identifier_statement = <identifier> | <identifier><equals_tk><assigment>

assigment = <function_declaration> | <expression>

expression = <expression_atomic> | <expression_atomic><operation><expression> | <left_parenthesis_tk><expression><left_parenthesis_tk>
expression_atomic = <identifier> | <string_literal> | <number_literal>

operation = 

function_declaration = <function_tk>|<arrow_function>

function = <function_tk><left_parenthesis_tk><params_list><left_parenthesis_tk><statement_block>

arrow_function = <arrow_function_params><equals_tk><gt_tk><statement_block>
arrow_function_params = <left_parenthesis_tk><params_list><left_parenthesis_tk> | <identifier>

params_list = <identifier> | <identifier><comma_tk><params_list>
```
