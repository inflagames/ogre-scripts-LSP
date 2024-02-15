#include <utility>

#include "../inc/logs.h"

Logs &Logs::getInstance() {
    static Logs instance;
    return instance;
}

void Logs::log(std::string text) {
#ifndef NDEBUG
    log(std::move(text), std::nullopt);
#endif // NDEBUG
}

void Logs::log(std::string text, std::optional<std::exception> e) {
#ifndef NDEBUG
    if (file.is_open()) {
        if (e.has_value()) {
            text.push_back('\n');
            text.append(e.value().what());
        }
        file << text << std::endl;
        file << "----------------------------------------" << std::endl;
    }
#endif // NDEBUG
}

void Logs::enableLogs(const std::string &logFile) {
#ifndef NDEBUG
    std::filesystem::path filePath(logFile);
    file.open(std::filesystem::temp_directory_path() / filePath, std::fstream::out | std::fstream::trunc);
#endif // NDEBUG
}
