#include <utility>

#include "../inc/lsp_server.h"

void OgreScriptLSP::LspServer::runServer(std::ostream &oos, std::istream &ios) {
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
                initialize(rm, oos);
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
                    didOpen(rm, oos);
                } else if ("textDocument/didClose" == rm->method) {
                    didClose(rm);
                } else if ("textDocument/didSave" == rm->method) {
                    // not needed at the moment
                    continue;
                } else if ("textDocument/didChange" == rm->method) {
                    didChange(rm, oos);
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

void OgreScriptLSP::LspServer::initialize(OgreScriptLSP::RequestMessage *rm, std::ostream &oos) {
//    clientCapabilities = ((InitializeParams *) rm)->capabilities;
    running = true;
    ResponseMessage re = newResponseMessage(rm->id, new InitializeResult());
    sendResponse(nlohmann::to_string(re.toJson()), oos);
}

void OgreScriptLSP::LspServer::didOpen(RequestMessage *rm, std::ostream &oos) {
    try {
        auto uri = ((DidOpenTextDocumentParams *) rm->params)->textDocument.uri;
        auto parser = getParserByUri(uri);
        sendDiagnostic(parser, oos);
    } catch (...) {
        // toDo (gonzalezext)[08.02.24]: exception
    }
}

void OgreScriptLSP::LspServer::didClose(RequestMessage *rm) {
    std::string uri = ((DidCloseTextDocumentParams *) rm->params)->textDocument.uri;
    auto it = parsers.find(uri);
    if (it != parsers.end()) {
        delete it->second;
        parsers.erase(it);
    }
}

void OgreScriptLSP::LspServer::didChange(RequestMessage *rm, std::ostream &oos) {
    auto uri = ((DidChangeTextDocumentParams *) rm->params)->textDocument.uri;
    auto parser = getParserByUri(uri);
    parser->parse(uri);

    sendDiagnostic(parser, oos);
}

void OgreScriptLSP::LspServer::goToDefinition(RequestMessage *rm, std::ostream &oos) {
    try {
        DefinitionParams *definitionParams = ((DefinitionParams *) rm->params);
        auto parser = getParserByUri(definitionParams->textDocument.uri);
        auto *res = parser->goToDefinition(definitionParams->position);

        ResponseMessage re = newResponseMessage(rm->id, res);
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (...) {
        // toDo (gonzalezext)[03.02.24]: send fail to client
        Logs::getInstance().log("ERROR: goToDefinition");
    }
}

void OgreScriptLSP::LspServer::formatting(RequestMessage *rm, bool withRange, std::ostream &oos) {
    try {
        auto parser = getParserByUri(((DocumentFormattingParams *) rm->params)->textDocument.uri);
        FormattingOptions options = ((DocumentFormattingParams *) rm->params)->options;
        ResultBase *res;
        if (withRange) {
            res = OgreScriptLSP::Formatter::formatting(parser, options,
                                                       ((DocumentRangeFormattingParams *) rm->params)->range);
        } else {
            res = OgreScriptLSP::Formatter::formatting(parser, options);
        }

        ResponseMessage re = newResponseMessage(rm->id, res);
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (OgreScriptLSP::BaseException e) {
        // toDo (gonzalezext)[03.02.24]: send fail message to client
        Logs::getInstance().log("ERROR: " + e.message);
    }
}

void OgreScriptLSP::LspServer::shutdown() {
    exit();
}

void OgreScriptLSP::LspServer::exit() {
}

OgreScriptLSP::Action OgreScriptLSP::LspServer::readHeaders(std::istream &os) {
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

std::string OgreScriptLSP::LspServer::readHeaderName(std::istream &os) {
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

std::string OgreScriptLSP::LspServer::readHeaderValue(std::istream &os) {
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

OgreScriptLSP::Action OgreScriptLSP::LspServer::readContent(Action action, std::istream &os) {
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

char OgreScriptLSP::LspServer::nextCharacter(std::istream &os) {
    if (os.eof()) {
        throw OgreScriptLSP::ServerEOFException("Client EOF error");
    }

    ch = (char) os.get();
    message.push_back(ch);
    return ch;
}

OgreScriptLSP::Parser *OgreScriptLSP::LspServer::getParserByUri(const std::string &uri) {
    OgreScriptLSP::Parser *parser;

    if (parsers.contains(uri)) {
        // get existing parser
        parser = parsers[uri];
    } else {
        // need to be created if not exist
        parser = new OgreScriptLSP::Parser();
        parsers[uri] = parser;
        parser->parse(uri);
    }
    return parser;
}

void OgreScriptLSP::LspServer::sendDiagnostic(Parser *parser, std::ostream &oos) {
    std::vector<Diagnostic> diagnostics;
    for (const auto &err: parser->getExceptions()) {
        diagnostics.push_back(Diagnostic{DIAGNOSTIC_SEVERITY_ERROR, err.range, err.message});
    }

    auto *params = new PublishDiagnosticsParams();
    params->uri = parser->uri;
    params->diagnostics = diagnostics;
    sendResponse(nlohmann::to_string(newNotificationMessage(NOTIFICATION_PUBLISH_DIAGNOSTICS, params).toJson()), oos);
}

void OgreScriptLSP::LspServer::sendResponse(const std::string &msg, // NOLINT(*-convert-member-functions-to-static)
                                            std::ostream &oos) {
    std::string header = HEADER_CONTENT_LENGTH;
    header += ":";
    header += std::to_string(msg.size());
    header += "\r\n";
    header += HEADER_CONTENT_TYPE;
    header += ": application/vscode; charset=utf-8\r\n\r\n";
    oos << header << msg;
    Logs::getInstance().log("Response: " + header + msg);
}

OgreScriptLSP::ResponseMessage
OgreScriptLSP::LspServer::newResponseMessage(std::string id, OgreScriptLSP::ResultBase *result) {
    ResponseMessage res;
    res.id = std::move(id);
    res.result = result;
    return res;
}

OgreScriptLSP::NotificationMessage
OgreScriptLSP::LspServer::newNotificationMessage(std::string method, OgreScriptLSP::ResultBase *params) {
    NotificationMessage notification(std::move(method));
    notification.params = params;
    return notification;
}
