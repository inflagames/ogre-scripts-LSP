#include "../inc/logs.h"

Logs &Logs::getInstance() {
    static Logs instance;
    return instance;
}

void Logs::log(std::string text) {
    if (file.is_open()) {
        file << text << std::endl;
        file << "----------------------------------------" << std::endl;
    }
}

void Logs::enableLogs(std::string logFile) {
#ifdef __linux__
    file.open("/var/log/" + logFile, std::fstream::out | std::fstream::trunc);
//    file.open("/home/gonzalezext/Desktop/" + logFile, std::fstream::out | std::fstream::trunc);
#elif _WIN32
    // toDo (gonzalezext)[08.02.24]: open logs file in window
#endif
}
