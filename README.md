# Ogre scripts LSP

This project is mean to create an LSP for ogre script (materials, fonts, particles, etc)

Base on the LSP specifications from
microsoft: https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

## Server Capabilities

- [x] textDocument/formatting (formatting requests)
- [x] textDocument/rangeFormatting (range formatting requests)
- [x] textDocument/definition (go to definition requests)
- [ ] textDocument/publishDiagnostics (diagnostic notification)
- [ ] textDocument/declaration (go to declaration request)
- [ ] textDocument/hover (hover information requests)
- [ ] textDocument/completion (code completion requests)

## Communication client-server supported

- stdio

## Build

```
# build project directly to /usr/local/bin/
./integration/build.sh
```

## Lexer Material

```
script = <script_body> | <script_body> <script>
script_body = <program> | <material>

// program definition
program = <program_type> <identifier> <program_opt> <left_curly_bracket_tk> <program_body> <right_curly_bracket_tk>
program_type = <fragment_program_tk> | <vertex_program_tk>
program_opt = <identifier> | <identifier> <program_opt>
program_body = <program_body_opt> | <program_body_opt> <program_body>
program_body_opt = <param_line> | <program_default>

program_default = <default_params_tk> <left_curly_bracket_tk> <program_default_body> <right_curly_bracket_tk>
program_default_body = <param_line> | <param_line> <program_default_body>

// import definition
import = <import_tk> <import_opt> <identifier> <string_literal>
import_opt = <asterisk_tk> <identifier>

// abstract block
abstract = <abstract_tk> <abstract_opt>
abstract_opt = <abstract_pass> | <abstract_technique> | <abstract_texture> | <abstract_material>
abstract_material = <material_tk> <identifier> <left_curly_bracket_tk> <material_body>* <right_curly_bracket_tk>
abstract_technique = <technique_tk> <identifier> <left_curly_bracket_tk> <material_technique_body>* <right_curly_bracket_tk>
abstract_pass = <pass_tk> <identifier> <left_curly_bracket_tk> <material_pass_body>* <right_curly_bracket_tk>
abstract_texture = <texture_unit_tk> <identifier> <left_curly_bracket_tk> <material_texture_body>* <right_curly_bracket_tk>

// .material scripts
material = <material_tk> <top_object_definition> <left_curly_bracket_tk> <material_body>* <right_curly_bracket_tk>
material_body = <material_technique> | <param_line>

material_technique = <technique_tk> <object_definition>? <left_curly_bracket_tk> <material_technique_body>* <right_curly_bracket_tk>
material_technique_body = <material_pass>

material_pass = <pass_tk> <object_definition>? <left_curly_bracket_tk> <material_pass_body>* <right_curly_bracket_tk>
material_pass_body = <param_line> | <material_texture> | <material_program>

material_texture = <texture_unit_tk> <object_definition>? <left_curly_bracket_tk> <material_texture_body>* <right_curly_bracket_tk>
material_texture_body = <param_line>

material_program = <material_progarm_type> <identifier> <left_curly_bracket_tk> <material_program_body>* <right_curly_bracket_tk>
material_program_type = <vertex_program_ref_tk> | <fragment_program_ref_tk>
material_program_body = <param_line>


// common implementations
param_line = <param><endl_tk> | <param> <param_line>
param = <identifier> | <string_literal> | <number_literal> | <variable>

object_definition = <top_object_definition> | <match_literal>
top_object_definition = <definition_opt> | <definition_opt> <colon_tk> <definition_opt>
definition_opt = <identifier> | <string_literal>
```

## ToDo

- [ ] competition support
- [ ] highlight support
- [ ] include http client-server communication support
