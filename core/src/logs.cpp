//
// Created by gonzalezext on 29.01.24.
//
#include "../inc/logs.h"

Logs &Logs::getInstance() {
    static Logs instance;
    return instance;
}

Logs::Logs(std::string logFile) {
//    file.open("/var/log/" + logFile, std::fstream::out | std::fstream::trunc);
    file.open("/home/gonzalezext/Desktop/" + logFile, std::fstream::out | std::fstream::trunc);
}

void Logs::log(std::string text) {
    if (file.is_open()) {
        file << text << std::endl;
        file << "----------------------------------------" << std::endl;
    }
}
