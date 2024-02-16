//
// Created by gonzalezext on 16.02.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H
#define OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H

#include <iostream>
#include <vector>
#include <string>

#define HELP "-h"
#define HELP_LONG "--help"
#define LOG "-l"
#define LOG_LONG "--logs"

class ProgramArg {
public:
    std::string programName;
    std::vector<std::string> args;

    ProgramArg(int argc, char **argv);

    ~ProgramArg();

    bool shouldShowHelp();

    bool shouldLogging();

    bool optionExists(std::string_view option);

    void showHelp() const;
};


#endif //OGRE_SCRIPTS_LSP_LIB_PROGRAM_ARG_H
