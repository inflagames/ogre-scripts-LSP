#include "program_arg.h"

ProgramArg::ProgramArg(int argc, const char **argv) {
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

bool ProgramArg::shouldShowVersion() {
    return optionExists(VERSION) || optionExists(VERSION_LONG);
}

bool ProgramArg::shouldLogging() {
    return optionExists(LOG) || optionExists(LOG_LONG);
}

bool ProgramArg::optionExists(std::string_view option) {
    return std::find(args.begin(), args.end(), option) != args.end();
}

void ProgramArg::showHelp() const {
    std::cout << programName << " [options]\n" <<
              "Options: \n" <<
              HELP << " | " << HELP_LONG << "         - Print program help\n" <<
              VERSION << " | " << VERSION_LONG << "      - Print program version\n" <<
              LOG << " | " << LOG_LONG
              << " [file]  - Logging to the specified file (optional) or to a random file in the system logs directory\n";
}

void ProgramArg::showVersion() const { // NOLINT(*-convert-member-functions-to-static)
    std::cout << APP_VERSION << std::endl;
}
