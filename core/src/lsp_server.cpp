//
// Created by gonzalezext on 25.01.24.
//

#include "../inc/lsp_server.h"
#include "../inc/formatter.h"

void LspServer::runServer(std::ostream &oos, std::istream &ios) {
    // run server until exit or crash
    while (true) {
        try {
            message = "";
            Action act = readHeaders(ios);
            act = readContent(act, ios);
            Logs::getInstance().log("Request: " + message);

            if (ios.eof()) {
                throw OgreScriptLSP::ServerEOFException("Client EOF");
            }

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
                    formatting(rm, false, oos);
                } else if ("textDocument/rangeFormatting" == rm->method) {
                    formatting(rm, true, oos);
                } else if ("textDocument/definition" == rm->method) {
                    goToDefinition(rm, oos);
                } else if ("textDocument/didOpen" == rm->method) {
                    // toDo (gonzalezext)[08.02.24]:
                } else if ("textDocument/didClose" == rm->method) {
                    // toDo (gonzalezext)[08.02.24]:
                } else if ("textDocument/didSave" == rm->method) {
                    // toDo (gonzalezext)[08.02.24]:
                } else if ("textDocument/didChange" == rm->method) {
                    // not needed at the moment
                } else if ("textDocument/declaration" == rm->method) {
                    // toDo (gonzalezext)[29.01.24]:
                }
            } else {
                shutdown();
            }
        } catch (OgreScriptLSP::ServerEOFException e) {
            Logs::getInstance().log("Error: " + e.message);
            break;
        } catch (...) {
            Logs::getInstance().log("Error: The server crash");
            break;
        }
    }
}

void LspServer::didOpen(RequestMessage *rm) {
    try {
        getParserByUri(((DidOpenTextDocumentParams *) rm->params)->textDocument.uri);
    } catch (...) {
        // toDo (gonzalezext)[08.02.24]: exception
    }
}

void LspServer::didClose(RequestMessage *rm) {
    std::string uri = ((DidCloseTextDocumentParams *) rm->params)->textDocument.uri;
    auto it = parsers.find(uri);
    if (it != parsers.end()) {
        delete it->second;
        parsers.erase(it);
    }
}

void LspServer::didChange(RequestMessage *rm) {
    std::string uri = ((DidChangeTextDocumentParams *) rm->params)->textDocument.uri;
    parsersMarkedAsUpdated.insert(uri);
}

void LspServer::goToDefinition(RequestMessage *rm, std::ostream &oos) {
    try {
        DefinitionParams *definitionParams = ((DefinitionParams *) rm->params);
        auto parser = getParserByUri(definitionParams->textDocument.uri);
        auto *res = parser->goToDefinition(definitionParams->position);

        ResponseMessage re;
        re.id = rm->id;
        re.result = res;

        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (...) {
        // toDo (gonzalezext)[03.02.24]: send fail to client
        Logs::getInstance().log("ERROR: goToDefinition");
    }
}

void LspServer::formatting(RequestMessage *rm, bool withRange, std::ostream &oos) {
    try {
        auto parser = getParserByUri(((DocumentFormattingParams *) rm->params)->textDocument.uri);
        FormattingOptions options = ((DocumentFormattingParams *) rm->params)->options;
        ResultBase *res;
        if (withRange) {
            res = OgreScriptLSP::Formatter::formatting(parser, options, ((DocumentRangeFormattingParams *) rm->params)->range);
        } else {
            res = OgreScriptLSP::Formatter::formatting(parser, options);
        }

        ResponseMessage re;
        re.id = rm->id;
        re.result = res;

        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (OgreScriptLSP::BaseException e) {
        // toDo (gonzalezext)[03.02.24]: send fail message to client
        Logs::getInstance().log("ERROR: " + e.message);
    }
}

void LspServer::shutdown() {
    // toDo (gonzalezext)[26.01.24]: stop running request and exit
    exit();
}

void LspServer::exit() {
    // toDo (gonzalezext)[26.01.24]:
}

void LspServer::sendResponse(std::string msg, std::ostream &oos) {
    std::string header = HEADER_CONTENT_LENGTH;
    header += ":";
    header += std::to_string(msg.size());
    header += "\r\n";
    header += HEADER_CONTENT_TYPE;
    header += ": application/vscode; charset=utf-8\r\n\r\n";
    oos << header << msg;
    Logs::getInstance().log("Response: " + header + msg);
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
    if (os.eof()) {
        throw OgreScriptLSP::ServerEOFException("Client EOF error");
    }

    ch = (char) os.get();
    message.push_back(ch);
    return ch;
}

OgreScriptLSP::Parser *LspServer::getParserByUri(const std::string &uri) {
    OgreScriptLSP::Parser *r;
    bool needUpdate = parsersMarkedAsUpdated.contains(uri);

    if (parsers.contains(uri)) {
        // get existing parser
        r = parsers[uri];
    } else {
        // need to be created if not exist
        r = new OgreScriptLSP::Parser();
        parsers[uri] = r;
        needUpdate = true;
    }

    if (needUpdate) {
        r->loadScript(uri);
        r->parse();
    }

    return r;
}
