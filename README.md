# Ogre scripts LSP

This project is mean to create an LSP for ogre script (materials, fonts, particles, etc)

## Build

```

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

// import
import = <import_tk> <import_opt> <identifier> <string_literal>
import_opt = <asterisk_tk> <identifier>

// .material scripts
material = material_tk <object_definition> <left_curly_bracket_tk> <material_body> <right_curly_bracket_tk> 

material_body = <material_body_opt> | <material_body_opt> <material_body>
material_body_opt = <material_technique> | <param_line>

material_technique = <technique_tk> <left_curly_bracket_tk> <material_technique_body> <right_curly_bracket_tk>
material_technique_body = <material_technique_body_opt> | <material_technique_body_opt> <material_technique_body>
material_technique_body_opt = <material_pass>

material_pass = <pass_tk> <material_pass_name> <left_curly_bracket_tk> <material_pass_body> <right_curly_bracket_tk>
material_pass_name = empty | <identifier> <colon_tk> <identifier> | <match_literal> | <identifier>
material_pass_body = <material_pass_body_opt> | <material_pass_body_opt> <material_pass_body>
material_pass_body_opt = <param_line> | <material_texture> | <material_program>

material_texture = <texture_unit_tk> <material_texture_name> <left_curly_bracket_tk> <material_texture_body> <right_curly_bracket_tk>
material_texture_name = empty | <identifier>
material_texture_body = <param_line> | <param_line> <material_texture_body>

material_program = <material_progarm_type> <identifier> <left_curly_bracket_tk> <material_program_body> <right_curly_bracket_tk>
material_program_type = <vertex_program_ref_tk> | <fragment_program_ref_tk>
material_program_body = <param_line> | <param_line> <material_program_body>


// common implementations
param_line = <param><endl_tk> | <param> <param_line>
param = <identifier> | <string_literal> | <number_literal> | <variable>

object_definition = <identifier> | <identifier> <colon_tk> <identifier>
```
