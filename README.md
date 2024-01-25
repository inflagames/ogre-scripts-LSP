# Ogre scripts LSP

This project is mean to create an LSP for ogre script (materials, fonts, particles, etc)

## ToDo list

- [ ] scanner
- [ ] parser

## Lexer

```
material = material_tk <object_definition> <left_curly_bracket_tk> <material_body> <right_curly_bracket_tk> 

material_body = <material_body_opt> | <material_body_opt> <material_body>
material_body_opt = <material_technique> | <params>

material_technique = <technique_tk> <left_curly_bracket_tk> <material_technique_body> <right_curly_bracket_tk>
material_technique_body = <material_technique_body_opt>
material_technique_body_opt = <material_pass>

material_pass = <pass_tk>  <left_curly_bracket_tk> <material_pass_body> <right_curly_bracket_tk>
material_pass_name = <colon_tk> <identifier> | <string_literal> | <identifier>
material_pass_body = <material_technique_body_opt>
material_pass_body_opt = <params>




params = <param><endl_tk> | <param> <params>
param = <identifier> | <string_literal> | <number_literal> | <variable>

object_definition = <identifier> | <identifier> <colon_tk> <identifier>
```
