#ifndef OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H
#define OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define HELP "-h"
#define HELP_LONG "--help"
#define LOG "-l"
#define LOG_LONG "--logs"
#define VERSION "-v"
#define VERSION_LONG "--version"

class ProgramArg {
public:
    std::string programName;
    std::vector<std::string> args;

    ProgramArg(int argc, const char **argv);

    ~ProgramArg();

    bool shouldShowHelp();

    bool shouldLogging();

    bool shouldShowVersion();

    bool optionExists(std::string_view option);

    void showHelp() const;

    void showVersion() const;
};


#endif //OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H
