# Version v0.0.2

Fixed general issues related to the scripting language and not supported tokens.

## Change Log

- Fixed: Several issues with formatting code with exceptions.
- Fixed: Removed not used tokens in the scanner (entry_point_tk, profiles_tk).
- Added: Support for missing <rtshader_system> block.
- Added: Basic support for semanticTokensProvider to the server.
- Added: Support for missing <geometry_program>|<geometry_program_ref> blocks.
- Added: Support for missing <tessellation_hull_program>|<tessellation_hull_program_ref> blocks.
- Added: Support for missing <tessellation_domain_program>|<tessellation_domain_program_ref> blocks.
- Added: Support for missing <compute_program>|<compute_program_ref> blocks.
- Added: Support for missing <shadow_receiver_material>|<shadow_caster_material> blocks.
- Fixed: Error with characters allowed in identifier tokens.
- Fixed: Issue with float numbers that ends with the character 'f'.
- Added: Support for missing <shared_params> block.
- Fixed: Issue with imports source token.
- Fixed: Issue with identifiers that start with digits.
