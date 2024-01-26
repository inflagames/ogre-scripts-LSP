//
// Created by gonzalezext on 25.01.24.
//

#include "../inc/LspServer.h"
#include "../inc/LspProtocol.h"

Action LspServer::readHeaders(std::istream &os) {
    Action msg = {0, "", new RequestMessage()};
    while (true) {
        std::string name = readHeaderName(os);

        if (name.empty()) {
            break;
        }

        std::string value = readHeaderValue(os);

        if ("Content-Length" == name) {
            msg.contentLength = std::stoi(value);
        } else if ("Content-Type" == name) {
            msg.contentType = value;
        }
    }
    return msg;
}

std::string LspServer::readHeaderName(std::istream &os) {
    std::string name;
    while (nextCharacter(os) != EOF) {
        if (ch == '\r') {
            nextCharacter(os);
            break;
        }
        if (ch == ':') {
            break;
        }
        name.push_back(ch);
    }
    return name;
}

std::string LspServer::readHeaderValue(std::istream &os) {
    std::string name;
    bool firstChar = true;
    while (nextCharacter(os) != EOF) {
        if (firstChar && ch == ' ') {
            firstChar = false;
            continue;
        }
        firstChar = false;
        if (ch == '\r') {
            nextCharacter(os);
            break;
        }
        name.push_back(ch);
    }
    return name;
}

Action LspServer::readContent(Action action, std::istream &os) {
    std::string jsonrpc;
    for (int i = 0; i < action.contentLength; ++i) {
        if (nextCharacter(os) == EOF) {
            break;
        }
        jsonrpc.push_back(ch);
    }

    // toDo (gonzalezext)[26.01.24]: if fail, send error to the client
    // convert content to json obj
//    nlohmann::json j = ""_json;
    nlohmann::json j = nlohmann::json::parse(jsonrpc);
    ((RequestMessage *) action.message)->fromJson(j);

    return action;
}

char LspServer::nextCharacter(std::istream &os) {
    ch = (char) os.get();
    return ch;
}
