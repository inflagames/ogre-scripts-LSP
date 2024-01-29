//
// Created by gonzalezext on 29.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LOGS_H
#define OGRE_SCRIPTS_LSP_LIB_LOGS_H

#include <fstream>
#include <cstdio>
#include <string>
#include <mutex>

class Logs {
private:
    std::ofstream file;

    explicit Logs(std::string logFile = "ogre3dscriptlsp.log");;
public:

    static Logs &getInstance();

    void log(std::string text);
};

#endif //OGRE_SCRIPTS_LSP_LIB_LOGS_H
