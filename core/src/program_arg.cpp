//
// Created by gonzalezext on 16.02.24.
//

#include "../inc/program_arg.h"

ProgramArg::ProgramArg(int argc, char **argv) {
    programName = std::string(argv[0]);
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
}

ProgramArg::~ProgramArg() {
    args.clear();
}

bool ProgramArg::shouldShowHelp() {
    return optionExists(HELP) || optionExists(HELP_LONG);
}

bool ProgramArg::shouldLogging() {
    return optionExists(LOG) || optionExists(LOG_LONG);
}

bool ProgramArg::optionExists(std::string_view option) {
    return std::find(args.begin(), args.end(), option) != args.end();
}

void ProgramArg::showHelp() const {
    // toDo (gonzalezext)[16.02.24]: need improvement
    std::cout << std::endl <<
              programName << " [options]\n" <<
              "Options: \n" <<
              HELP << " | " << HELP_LONG << "         - Print program help\n" <<
              LOG << " | " << LOG_LONG
              << " [file]  - Logging to the specified file (optional) or to a random file in the system logs directory\n";
}
