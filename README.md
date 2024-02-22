# Ogre scripts LSP

This project is an LSP (Language Server Protocol) for ogre3d scripts (materials, fonts, particles, etc)

Base on the LSP specifications from
microsoft: https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/.

Language specifications can be found on the ogre3d
documentation: https://ogrecave.github.io/ogre/api/latest/_scripts.html.

## IDE integration

### CLion (JetBrain)

For CLion you can use the plugin [Ogre3d Scripts](https://plugins.jetbrains.com/plugin/23756-ogre3d-scripts/edit)

> Note: The plugin repository can be found here: https://github.com/inflagames/ogre3dscript-jetbrain-plugin

### NeoVim

Use the next configuration to setting up the LS in nvim:

```lua
vim.filetype.add({
    extension = {
        material = "material",
    },
})

vim.api.nvim_create_autocmd('FileType', {
    desc = 'LSP start',
    pattern = { "material" },
    callback = function()
        print("Started server")
        vim.lsp.start({
            name = 'ogre3d-script-lsp',
            cmd = { '~/ogre_scripts_LSP', '-l' },
            root_dir = vim.fs.dirname(vim.fs.find({ 'resources.cfg' }, { upward = true })[1])
        })
    end
})
```

> Note: the **cmd** option is pointing to the LSP binary on home.
> Be sure you have downloaded the plugin latest release to this folder.

### Visual Studio

TBD

### Visual Studio Code

TBD

## Languages supported (so far)

- [x] .material
- [ ] .program
- [ ] .particle
- [ ] .compositor
- [ ] .os
- [ ] .overlay

> Note: work in progress

## Server Capabilities

- [x] textDocument/formatting (formatting requests)
- [x] textDocument/rangeFormatting (range formatting requests)
- [x] textDocument/definition (go to definition requests)
- [x] textDocument/publishDiagnostics (diagnostic notification)
- [ ] textDocument/declaration (go to declaration request)
- [ ] textDocument/hover (hover information requests)
- [ ] textDocument/completion (code completion requests)

> Note: work in progress

## Communication client-server supported

- stdio

## Build

This command will build the application directly on your `/usr/local/bin/` directory.

```
# build project directly to /usr/local/bin/
./integration/build.sh
```

## Lexer definition

### .material

```
script = <script_body> | <script_body> <script>
script_body = <program> | <material>

// program definition
program = <program_type> <identifier> <identifier>* <left_curly_bracket_tk> <program_body>* <right_curly_bracket_tk>
program_type = <fragment_program_tk> | <vertex_program_tk> | <geometry_program_tk> | <tessellation_hull_program_tk> | <tessellation_domain_program_tk> | <compute_program_tk> 
program_body = <param_line> | <program_default>

program_default = <default_params_tk> <left_curly_bracket_tk> <program_default_body>* <right_curly_bracket_tk>
program_default_body = <param_line> | <shared_params_ref>

// import definition
import = <import_tk> <import_opt> <from_tk> <import_source>
import_opt = <asterisk_tk> | <identifier>
import_source = <string_literal> | <identifier>

// shared params definition
shared_params = <shared_params_tk> <identifier> <left_curly_bracket_tk> <shared_params_body>* <right_curly_bracket_tk>
shared_params_body = <param_line>

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
material_technique_body = <material_pass> | <param_line>
technique_shadow_material = <technique_shadow_material_type> <idendifier>
technique_shadow_material_type = <shadow_receiver_material_tk> | <shadow_caster_material_tk>

material_pass = <pass_tk> <object_definition>? <left_curly_bracket_tk> <material_pass_body>* <right_curly_bracket_tk>
material_pass_body = <param_line> | <material_texture> | <material_program>

material_texture = <texture_unit_tk> <object_definition>? <left_curly_bracket_tk> <material_texture_body>* <right_curly_bracket_tk>
material_texture_body = <param_line> | <material_rtshader> | <material_texture_source>

material_rtshader = <rtshader_system_tk> <object_definition>? <left_curly_bracket_tk> <material_rtshader_body>* <right_curly_bracket_tk>
material_rtshader_body = <param_line>

material_texture_source = <texture_source_tk> <object_definition>? <left_curly_bracket_tk> <material_texture_source_body>* <right_curly_bracket_tk>
material_texture_source_body = <param_line>

material_program = <material_progarm_type> <identifier> <left_curly_bracket_tk> <material_program_body>* <right_curly_bracket_tk>
material_program_type = <vertex_program_ref_tk> | <fragment_program_ref_tk> | <geometry_program_ref_tk> | <tessellation_hull_program_ref_tk> | <tessellation_domain_program_ref_tk> | <compute_program_ref_tk>
material_program_body = <param_line> | <shared_params_ref>
shared_params_ref = <shared_params_ref_tk> <identifier>

// common implementations
param_line = <param><endl_tk> | <param> <param_line>
param = <identifier> | <string_literal> | <number_literal> | <variable> | <comma_tk>

object_definition = <top_object_definition> | <match_literal>
top_object_definition = <definition_opt> | <definition_opt> <colon_tk> <definition_opt>
definition_opt = <identifier> | <string_literal>
```

## ToDo

- [ ] ci/cd build and deploy released versions
- [ ] competition support
- [ ] highlight support
- [ ] include http client-server communication support

## Contribution

Any contribution is welcome.

## License

tbd
