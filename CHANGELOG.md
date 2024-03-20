# Version v0.0.3

Fixed issues with memory leaks in the code and add support for validate params in the different logic blocks (pass, material, technique, textureUnit, sample).

## Change Log

- Changed: Restructure files in project and remove unused methods
- Fix: Update all pointer uses in ast_tree to avoid memory leaks
- Fix: Update all pointers around the LSP to avoid memory leaks
- Added: Include support for pass params validation
- Added: Include support for material params validation
- Added: Include support for technique params validation
- Fix: Issue with allow match_literals on params list
- Added: Include support for texture unit params validation
- Added: Include support for sample params validation
