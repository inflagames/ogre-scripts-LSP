//
// Created by gonzalezext on 25.01.24.
//

#include "../inc/LspServer.h"

void LspServer::runServer(std::ostream &oos, std::istream &ios) {
    while (true) {
        Action act = readHeaders(ios);
        act = readContent(act, ios);

        auto *rm = (RequestMessage *) act.message;
        if ("initialize" == rm->method) {
            // this action is sync
            ResponseMessage re;
            re.id = rm->id;
            re.result = new InitializeResult();

            nlohmann::json resJson = re.toJson();
            running = true;
            sendResponse(nlohmann::to_string(resJson), oos);
        } else if ("initialized" == rm->method) {
            // toDo (gonzalezext)[26.01.24]: check if something need to be done
            // client confirmation that received the InitializeResponse
        } else if ("shutdown" == rm->method) {
            shutdown();
            break;
        } else if ("exit" == rm->method) {
            exit();
            break;
        } else if (running) {
            if ("textDocument/formatting" == rm->method) {
                // toDo (gonzalezext)[26.01.24]:
            }
        } else {
            shutdown();
        }
    }
}

void LspServer::shutdown() {
    // toDo (gonzalezext)[26.01.24]: stop running request and exit
    exit();
}

void LspServer::exit() {
    // toDo (gonzalezext)[26.01.24]:
}

void LspServer::sendResponse(std::string msj, std::ostream &oos) {
    std::string header = HEADER_CONTENT_LENGTH;
    header += ":";
    header += std::to_string(msj.size());
    header += "\r\n";
    header += HEADER_CONTENT_TYPE;
    header += ": application/vscode; charset=utf-8\r\n\r\n";
    oos << header << msj;
}

Action LspServer::readHeaders(std::istream &os) {
    Action msg = {0, "", new RequestMessage()};
    while (true) {
        std::string name = readHeaderName(os);

        if (name.empty()) {
            break;
        }

        std::string value = readHeaderValue(os);

        if (HEADER_CONTENT_LENGTH == name) {
            msg.contentLength = std::stoi(value);
        } else if (HEADER_CONTENT_TYPE == name) {
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

    try {
        // convert content to json obj
        nlohmann::json j = nlohmann::json::parse(jsonrpc);
        ((RequestMessage *) action.message)->fromJson(j);
    } catch (...) {
        // toDo (gonzalezext)[26.01.24]: handle error here
    }

    return action;
}

char LspServer::nextCharacter(std::istream &os) {
    ch = (char) os.get();
    return ch;
}
