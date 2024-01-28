#include <utility>

//
// Created by gonzalezext on 28.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
#define OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H

// Error messages
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

#define FILE_IS_NOT_OPEN_OR_NOT_EXIST "File is not open or not exist"

namespace OgreScriptLSP {
    struct BaseException : std::exception {
    public:
        std::string message;
        int line;
        int column;

        BaseException(std::string message, int line, int column)
                : message(std::move(message)), line(line), column(column) {}
    };

    struct ParseException : public BaseException {
    public:
        explicit ParseException(std::string message, int line, int column)
                : BaseException(std::move(message), line, column) {}
    };

    struct FileException : public BaseException {
    public:
        explicit FileException(std::string message)
                : BaseException(std::move(message), 0, 0) {}
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
