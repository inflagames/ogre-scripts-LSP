#include "lsp_server.h"
#include "lang/params_validator.h"

void OgreScriptLSP::LspServer::runServer(std::ostream &oos, std::istream &ios) {
    // run server until exit or crash
    while (true) {
        try {
            message = "";
            Action act = readHeaders(ios);
            readContent(act, ios);
            Logs::getInstance().log("Request: " + message);

            if (ios.eof()) {
                throw OgreScriptLSP::ServerEOFException("Client EOF");
            }

            auto *rm = (RequestMessage *) act.message.get();
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
                } else if ("textDocument/declaration" == rm->method) {
                } else if ("textDocument/didOpen" == rm->method) {
                    didOpen(rm, oos);
                } else if ("textDocument/didClose" == rm->method) {
                    didClose(rm);
                } else if ("textDocument/didSave" == rm->method) {
                    // not used at the moment
                    continue;
                } else if ("textDocument/didChange" == rm->method) {
                    didChange(rm, oos);
                } else if ("textDocument/documentSymbol" == rm->method) {
                    documentSymbols(rm, oos);
                } else if (rm->method.starts_with("textDocument/semanticTokens")) {
                    semanticTokens(rm, oos);
                }
            } else {
                shutdown();
            }
        } catch (OgreScriptLSP::ServerEOFException &e) {
            Logs::getInstance().log("Error: " + e.message, e);
            break;
        } catch (std::exception &e) {
            Logs::getInstance().log("Error: The server crash", e);
            break;
        }
    }
    running = false;
}

void OgreScriptLSP::LspServer::initialize(OgreScriptLSP::RequestMessage *rm, std::ostream &oos) {
//    clientCapabilities = ((InitializeParams *) rm)->capabilities;
    running = true;
    ResponseMessage re = newResponseMessage(rm->id, new InitializeResult());
    sendResponse(nlohmann::to_string(re.toJson()), oos);
}

void OgreScriptLSP::LspServer::semanticTokens(RequestMessage *rm, std::ostream &oos) {
    try {
        auto params = (SemanticTokensParams *) rm->params.get();
        auto parser = getParserByUri(params->textDocument.uri);
        Range range = {{0, 0},
                       {INT32_MAX, INT32_MAX}};
        if (rm->method.ends_with("/delta")) {
            // toDo: not supported yet
        } else if (rm->method.ends_with("/range")) {
            range = ((SemanticTokensRangeParams *) params)->range;
        }
        ResponseMessage re = newResponseMessage(rm->id, SemanticToken::getSemanticTokens(parser, range));
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (std::exception &e) {
        // toDo (gonzalezext)[21.02.24]: send error to client
        Logs::getInstance().log("Error calculating semantic tokens", e);
    }
}

void OgreScriptLSP::LspServer::documentSymbols(RequestMessage *rm, std::ostream &oos) {
    try {
        auto params = (DocumentSymbolParams *) rm->params.get();
        auto parser = getParserByUri(params->textDocument.uri);
        ResponseMessage re = newResponseMessage(rm->id, Symbols::getSymbols(parser));
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (std::exception &e) {
        // toDo (gonzalezext)[21.02.24]: send error to client
        Logs::getInstance().log("Error calculating symbols", e);
    }
}

void OgreScriptLSP::LspServer::didOpen(RequestMessage *rm, std::ostream &oos) {
    try {
        auto params = (DidOpenTextDocumentParams *) rm->params.get();
        sendDiagnostic(updateParser(params->textDocument.uri, params->textDocument.text), oos);
    } catch (std::exception &e) {
        // toDo (gonzalezext)[21.02.24]: send error to client
        Logs::getInstance().log("Error to open a file", e);
    }
}

void OgreScriptLSP::LspServer::didChange(RequestMessage *rm, std::ostream &oos) {
    // toDo (gonzalezext)[10.02.24]: not support for range changes
    auto params = (DidChangeTextDocumentParams *) rm->params.get();
    sendDiagnostic(updateParser(params->textDocument.uri, params->contentChanges[0].text), oos);
}

OgreScriptLSP::Parser *OgreScriptLSP::LspServer::updateParser(const std::string &uri, const std::string &code) {
    auto parser = new Parser();
    parser->loadScript(uri, code);
    parser->parse();
    updateParserByUri(uri, parser);
    return parser;
}

void OgreScriptLSP::LspServer::didClose(RequestMessage *rm) {
    std::string uri = ((DidCloseTextDocumentParams *) rm->params.get())->textDocument.uri;
    parsers.erase(uri);
}

void OgreScriptLSP::LspServer::goToDefinition(RequestMessage *rm, std::ostream &oos) {
    try {
        auto definitionParams = (DefinitionParams *) rm->params.get();
        auto parser = getParserByUri(definitionParams->textDocument.uri);
        auto res = OgreScriptLSP::GoTo::goToDefinition(parser->getScript(), parser->getDeclarations(),
                                                       definitionParams->position);

        ResponseMessage re = newResponseMessage(rm->id, res.release());
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (std::exception &e) {
        // toDo (gonzalezext)[03.02.24]: send fail to client
        Logs::getInstance().log("ERROR: goToDefinition", e);
    }
}

void OgreScriptLSP::LspServer::formatting(RequestMessage *rm, bool withRange, std::ostream &oos) {
    try {
        auto parser = getParserByUri(((DocumentFormattingParams *) rm->params.get())->textDocument.uri);
        FormattingOptions options = ((DocumentFormattingParams *) rm->params.get())->options;
        ResultBase *res;
        if (withRange) {
            res = OgreScriptLSP::Formatter::formatting(parser, options,
                                                       ((DocumentRangeFormattingParams *) rm->params.get())->range);
        } else {
            res = OgreScriptLSP::Formatter::formatting(parser, options);
        }

        ResponseMessage re = newResponseMessage(rm->id, res);
        sendResponse(nlohmann::to_string(re.toJson()), oos);
    } catch (OgreScriptLSP::BaseException &e) {
        // toDo (gonzalezext)[03.02.24]: send fail message to client
        Logs::getInstance().log("ERROR: " + e.message, e);
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
        if (ch == '\n') {
            break;
        }
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
        if (ch == '\n') {
            break;
        }
        if (ch == '\r') {
            nextCharacter(os);
            break;
        }
        name.push_back(ch);
    }
    return name;
}

void OgreScriptLSP::LspServer::readContent(Action &action, std::istream &os) {
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
        ((RequestMessage *) action.message.get())->fromJson(j);
    } catch (nlohmann::json::exception &e) {
        Logs::getInstance().log("Error with nlohmann::json lib", e);
        // toDo (gonzalezext)[26.01.24]: handle error here
    }
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
    if (!parsers.contains(uri)) {
        parsers[uri] = std::make_unique<OgreScriptLSP::Parser>();
        parsers[uri]->parse(uri);

        // validate params
        ParamsValidator::getSingleton()->paramsAnalysis(parsers[uri].get());
    }
    return parsers[uri].get();
}

void OgreScriptLSP::LspServer::updateParserByUri(const std::string &uri, OgreScriptLSP::Parser *parser) {
    parsers[uri] = std::unique_ptr<OgreScriptLSP::Parser>(parser);

    // validate params
    ParamsValidator::getSingleton()->paramsAnalysis(parsers[uri].get());
}

void OgreScriptLSP::LspServer::sendDiagnostic(Parser *parser, std::ostream &oos) {
    std::vector<Diagnostic> diagnostics;
    for (const auto &err: *parser->getExceptions()) {
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
    res.result = std::unique_ptr<ResultBase>(result);
    return res;
}

OgreScriptLSP::NotificationMessage
OgreScriptLSP::LspServer::newNotificationMessage(std::string method, OgreScriptLSP::ResultBase *params) {
    NotificationMessage notification(std::move(method));
    notification.params = std::unique_ptr<ResultBase>(params);
    return notification;
}
