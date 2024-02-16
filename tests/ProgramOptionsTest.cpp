#include "gtest/gtest.h"

#include "../core/inc/program_arg.h"

TEST (ProgramOptionsTest, parseCorrectlyTheArguments) {
    const char *args[] = {"myapp", HELP, HELP_LONG, LOG, LOG_LONG};
    auto *program = new ProgramArg(5, args);

    ASSERT_EQ(4, program->args.size());

    for (int i = 1; i <= 4; i++) {
        ASSERT_TRUE(program->optionExists(std::string(args[i])));
    }
}

TEST (ProgramOptionsTest, handleEmptyArguments) {
    const char *args[] = {"myapp"};
    auto *program = new ProgramArg(1, args);

    ASSERT_TRUE(program->args.empty());
}

TEST (ProgramOptionsTest, validateIfShowHelp) {
    const char *args[] = {"myapp", LOG, "adsf", "cvxc", HELP};
    auto *program = new ProgramArg(5, args);
    ASSERT_TRUE(program->shouldShowHelp());

    delete program;
    const char *argse[] = {"myapp", LOG, "adsf", "cvxc", HELP_LONG};
    program = new ProgramArg(5, argse);
    ASSERT_TRUE(program->shouldShowHelp());
}

TEST (ProgramOptionsTest, validateIfLogging) {
    const char *args[] = {"myapp", LOG, "adsf", "cvxc", HELP};
    auto *program = new ProgramArg(5, args);
    ASSERT_TRUE(program->shouldLogging());

    delete program;
    const char *argse[] = {"myapp", LOG_LONG, "adsf", "cvxc", HELP_LONG};
    program = new ProgramArg(5, argse);
    ASSERT_TRUE(program->shouldLogging());
}
