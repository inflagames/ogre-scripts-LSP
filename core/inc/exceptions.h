#ifndef OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
#define OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H

// Error messages
#include "lsp_protocol.h"

#define INVALID_TOKEN "Invalid main token"
#define PROGRAM_NAME_MISSION "Error with program name identifier"
#define MATERIAL_NAME_MISSION_ERROR "Error with material name identifier"
#define MATERIAL_PASS_NAME_MISSION_ERROR "Error with material pass name identifier"
#define MATERIAL_TEXTURE_NAME_MISSION_ERROR "Error with material texture name identifier"
#define MATERIAL_PROGRAM_NAME_MISSION_ERROR "Error with material program name identifier"
#define MATERIAL_INHERIT_ERROR "Error with material inherit"
#define MATERIAL_PASS_INHERIT_ERROR "Error with material pass inherit"
#define CURLY_BRACKET_START_MISSING "Missing start curly bracket"
#define CURLY_BRACKET_END_MISSING "Missing curly bracket"
#define PROGRAM_HIGH_LEVEL_MISSING "Missing high level program definition"
#define NOT_VALID_PROGRAM_PARAM "Not valid program param"
#define NOT_VALID_MATERIAL_PARAM "Not valid material param"
#define NOT_VALID_MATERIAL_PASS_PARAM "Not valid material pass param"
#define NOT_VALID_MATERIAL_TEXTURE_PARAM "Not valid material texture param"
#define NOT_VALID_MATERIAL_PROGRAM_PARAM "Not valid material program param"
#define NOT_VALID_MATERIAL_TECHNIQUE_BODY "Not valid material technique body"
#define NOT_VALID_PARAM "Not valid program default param"

// Scanner error messages
#define SCANNER_INVALID_CHARACTER "Invalid character"
#define SCANNER_INVALID_NUMBER "Invalid number"
#define SCANNER_INVALID_STRING_LITERAL "Invalid string literal"
#define SCANNER_INVALID_MATCH_LITERAL "Invalid match literal"
#define SCANNER_EOF_ERROR "EOF error"

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
