#ifndef OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
#define OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H

// Error messages
#include "lsp_protocol.h"

#define INVALID_TOKEN "Invalid main token"
#define PROGRAM_NAME_MISSING "Error with program name identifier"
#define SHARED_PARAMS_NAME_MISSING "Error with shared_params name identifier"
#define MATERIAL_NAME_MISSION_ERROR "Error with material name identifier"
#define TECHNIQUE_NAME_MISSION_ERROR "Error with technique name identifier"
#define PASS_NAME_MISSION_ERROR "Error with pass name identifier"
#define TEXTURE_NAME_MISSION_ERROR "Error with texture name identifier"
#define RTSHADER_MISSING_ERROR "Error with rtshader name identifier"
#define TEXTURE_SOURCE_MISSING_ERROR "Error with texture_source name identifier"
#define MATERIAL_PASS_NAME_MISSION_ERROR "Error with material pass name identifier"
#define MATERIAL_PROGRAM_NAME_MISSING_ERROR "Error with material program name identifier"
#define MATERIAL_INHERIT_ERROR "Error with material inherit"
#define TECHNIQUE_INHERIT_ERROR "Error with technique inherit"
#define PASS_INHERIT_ERROR "Error with pass inherit"
#define TEXTURE_INHERIT_ERROR "Error with texture inherit"
#define RTSHADER_INHERIT_ERROR "Error with rtshader inherit"
#define TEXTURE_SOURCE_INHERIT_ERROR "Error with texture_source inherit"
#define MATERIAL_PASS_INHERIT_ERROR "Error with material pass inherit"
#define CURLY_BRACKET_START_MISSING "Missing start curly bracket"
#define CURLY_BRACKET_END_MISSING "Missing curly bracket"
#define PROGRAM_HIGH_LEVEL_MISSING "Missing high level program definition"
#define NOT_VALID_PROGRAM_PARAM "Not valid program param"
#define NOT_VALID_SAMPLER_PARAM "Not valid sampler param"
#define NOT_VALID_SHARED_PARAMS_PARAM "Not valid shared_params param"
#define NOT_VALID_MATERIAL_PARAM "Not valid material param"
#define NOT_VALID_MATERIAL_PASS_PARAM "Not valid pass param"
#define NOT_VALID_MATERIAL_TEXTURE_PARAM "Not valid texture_unit param"
#define NOT_VALID_MATERIAL_RTSHADER_PARAM "Not valid rtshader param"
#define NOT_VALID_MATERIAL_TEXTURE_SOURCE_PARAM "Not valid texture_source param"
#define NOT_VALID_MATERIAL_PROGRAM_PARAM "Not valid program_ref param"
#define NOT_VALID_MATERIAL_TECHNIQUE_BODY "Not valid technique body"
#define NOT_VALID_SHADOW_MATERIAL_REF "Not valid shadow (caster/receiver) material reference"
#define NOT_VALID_SAMPLER_REF "Not valid sampler reference"
#define NOT_VALID_PARAM "Not valid param"
#define NOT_VALID_IMPORT "Invalid import declaration"
#define NOT_VALID_SHARED_PARAM_REF_IDENTIFIER "Invalid shared param ref identifier"

// Scanner error messages
#define SCANNER_INVALID_CHARACTER "Invalid character"
#define SCANNER_INVALID_NUMBER "Invalid number"
#define SCANNER_INVALID_STRING_LITERAL "Invalid string literal"
#define SCANNER_INVALID_MATCH_LITERAL "Invalid match literal"
#define SCANNER_EOF_ERROR "EOF error"

// Params exceptions
#define PARAM_MATERIAL_ERROR "Invalid material param"
#define PARAM_TECHNIQUE_ERROR "Invalid technique param"
#define PARAM_PASS_ERROR "Invalid pass param"
#define PARAM_TEXTURE_UNIT_ERROR "Invalid texture unit param"

// General error messages
#define FILE_IS_NOT_OPEN_OR_NOT_EXIST "File is not open or not exist"

namespace OgreScriptLSP {
    struct BaseException : std::exception {
    public:
        std::string message;
        Range range;

        BaseException(std::string message, Range range)
                : message(std::move(message)), range(range) {}
    };

    struct ParamsException : public BaseException {
    public:
        explicit ParamsException(std::string message, Range range)
                : BaseException(std::move(message), range) {}
    };

    struct ParseException : public BaseException {
    public:
        explicit ParseException(std::string message, Range range)
                : BaseException(std::move(message), range) {}
    };

    struct FileException : public BaseException {
    public:
        explicit FileException(std::string message)
                : BaseException(std::move(message), {}) {}
    };

    struct ScannerException : public BaseException {
    public:
        explicit ScannerException(std::string message, Range range)
                : BaseException(std::move(message), range) {}
    };

    struct ServerEOFException : public BaseException {
    public:
        explicit ServerEOFException(std::string message) : BaseException(std::move(message), {}) {}
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
