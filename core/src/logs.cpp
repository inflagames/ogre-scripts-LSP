#include <memory>
#include <utility>

#include "../inc/logs.h"

Logs &Logs::getInstance() {
    static Logs instance;
    return instance;
}

void Logs::log(std::string text) {
    if (file && file->is_open()) {
        log(std::move(text), std::nullopt);
    }
}

void Logs::log(std::string text, std::optional<std::exception> e) {
    if (file && file->is_open()) {
        if (e.has_value()) {
            text.push_back('\n');
            text.append(e.value().what());
        }
        (*file) << text << '\n';
        (*file) << "----------------------------------------" << std::endl;
    }
}

void Logs::enableLogs(const std::string &logFile) {
    std::filesystem::path filePath(logFile);
    file = std::make_unique<std::ofstream>(std::filesystem::temp_directory_path() / filePath,
                                           std::fstream::out | std::fstream::trunc);
}
