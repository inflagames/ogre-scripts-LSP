#ifndef OGRE_SCRIPTS_LSP_LIB_LOGS_H
#define OGRE_SCRIPTS_LSP_LIB_LOGS_H

#include <fstream>
#include <cstdio>
#include <string>
#include <mutex>
#include <optional>
#include <filesystem>
#include <iostream>

class Logs {
private:
    std::ofstream file;

    Logs() = default;

public:
    Logs(Logs &other) = delete;

    void operator=(const Logs &) = delete;

    static Logs &getInstance();

    void enableLogs(const std::string& logFile = "ogre3dscriptlsp.log");

    void log(std::string text);

    void log(std::string text, std::optional<std::exception> e);
};

#endif //OGRE_SCRIPTS_LSP_LIB_LOGS_H
