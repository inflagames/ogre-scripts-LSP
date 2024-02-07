//
// Created by gonzalezext on 26.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H
#define OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H

#include <iostream>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"
#include "logs.h"

struct Message {
    std::string jsonrpc;

    explicit Message(std::string jsonrpc) : jsonrpc(std::move(jsonrpc)) {}

    virtual void fromJson(const nlohmann::json &j) = 0;

    virtual nlohmann::json toJson() = 0;
};

struct TextDocumentIdentifier {
    std::string uri;
};

struct Position {
    int line;
    int character;

    bool operator<(const Position &a) const {
        return std::tie(line, character) < std::tie(a.line, a.character);
    }

    bool operator>(const Position &a) const {
        return std::tie(line, character) > std::tie(a.line, a.character);
    }
};

struct Range {
    Position start;
    Position end;

    [[nodiscard]] bool inRange(const Range &a) const {
        return !(a.start < start || a.end < start || a.start > end || a.end > end);
    }

    [[nodiscard]] bool inRange(const Position &a) const {
        return !(start > a || end < a);
    }
};

struct ParamsBase {
    virtual void fromJson(const nlohmann::json &j) {};
};

struct WorkDoneProgressParams : ParamsBase {
    std::string workDoneToken;
};

struct TextDocumentPositionParams {
    Position position{};
    TextDocumentIdentifier textDocument{};
};

struct PartialResultParams {
    std::string partialResultToken;
};

struct ResultBase {
    virtual nlohmann::json toJson() {
        return {};
    };
};

struct ResultArray : ResultBase {
    std::vector<ResultBase *> elements;

    nlohmann::json toJson() override {
        nlohmann::json array = nlohmann::json::array();
        for (auto ele: elements) {
            std::cout << ele->toJson() << std::endl;
            array.push_back(ele->toJson());
        }
        return array;
    }
};

struct FormattingOptions {
    uint32_t tabSize = 2;
    bool insertSpaces = true;
    bool trimTrailingWhitespace = true;
    bool insertFinalNewline = false;
    bool trimFinalNewLines = false;
};

struct Location : ResultBase {
    std::string uri;
    Range range;

    Location(std::string uri, Range range) : uri(std::move(uri)), range(range) {}

    nlohmann::json toJson() override {
        return nlohmann::json{
                {"uri",   uri},
                {"range", {
                                  {"start", {{"line", range.start.line}, {"character", range.start.character}}},
                                  {"end", {{"line", range.end.line}, {"character", range.end.character}}}
                          }}
        };
    }
};

struct TextEdit : ResultBase {
    Range range;
    std::string newText;

    TextEdit(Position start, Position end, std::string newText) : range({start, end}), newText(std::move(newText)) {}

    nlohmann::json toJson() override {
        return nlohmann::json{
                {"range",   {
                                    {"start", {{"line", range.start.line}, {"character", range.start.character}}},
                                    {"end", {{"line", range.end.line}, {"character", range.end.character}}}
                            }},
                {"newText", newText}
        };
    }
};

/**
 * '$/cancelRequest'
 */
struct CancelParams : ParamsBase {
    std::string id;
};

/**
 * 'initialize'
 */
struct InitializeParams : WorkDoneProgressParams {
    int processId{};
    std::string rootPath; // probably not used
    std::string rootUri; // probably not used
    struct ClientCapabilities {
        bool workspaceFolders;
        struct TextDocumentClientCapabilities {
            // toDo (gonzalezext)[26.01.24]:
        } textDocument;
        struct Window {
            // toDo (gonzalezext)[26.01.24]:
        } window;
    } capabilities{};
    struct WorkspaceFolder {
        std::string uri;
        std::string name;
    };
    std::vector<WorkspaceFolder> workspaceFolders;

    void fromJson(const nlohmann::json &j) override {
        if (j.contains("processId")) {
            j.at("processId").get_to(processId);
        }
        if (j.contains("rootPath")) {
            j.at("rootPath").get_to(rootPath);
        }
        if (j.contains("rootUri")) {
            j.at("rootUri").get_to(rootUri);
        }
    };
};

/**
 * 'textDocument/formatting' request
 */
struct DocumentFormattingParams : WorkDoneProgressParams {
    TextDocumentIdentifier textDocument;
    FormattingOptions options;

    void fromJson(const nlohmann::json &j) override {
        if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
            j.at("textDocument").at("uri").get_to(textDocument.uri);
        }
        if (j.contains("options")) {
            if (j.at("options").contains("tabSize")) {
                j.at("options").at("tabSize").get_to(options.tabSize);
            }
            if (j.at("options").contains("insertSpaces")) {
                j.at("options").at("insertSpaces").get_to(options.insertSpaces);
            }
            if (j.at("options").contains("trimTrailingWhitespace")) {
                j.at("options").at("trimTrailingWhitespace").get_to(options.trimTrailingWhitespace);
            }
            if (j.at("options").contains("insertFinalNewline")) {
                j.at("options").at("insertFinalNewline").get_to(options.insertFinalNewline);
            }
            if (j.at("options").contains("trimFinalNewLines")) {
                j.at("options").at("trimFinalNewLines").get_to(options.trimFinalNewLines);
            }
        }
    }
};

/**
 * 'textDocument/rangeFormatting' request
 */
struct DocumentRangeFormattingParams : DocumentFormattingParams {
    Range range{};

    void fromJson(const nlohmann::json &j) override {
        DocumentFormattingParams::fromJson(j);
        if (j.contains("range")) {
            j.at("range").at("start").at("line").get_to(range.start.line);
            j.at("range").at("start").at("character").get_to(range.start.character);
            j.at("range").at("end").at("line").get_to(range.end.line);
            j.at("range").at("end").at("character").get_to(range.end.character);
        }
    }
};

/**
 * 'textDocument/definition' request
 */
struct DefinitionParams : WorkDoneProgressParams, TextDocumentPositionParams, PartialResultParams {
    void fromJson(const nlohmann::json &j) override {
        if (j.contains("textDocument")) {
            j.at("textDocument").at("uri").get_to(textDocument.uri);
        }
        if (j.contains("position")) {
            j.at("position").at("line").get_to(position.line);
            j.at("position").at("character").get_to(position.character);
        }
        if (j.contains("workDoneToken")) {
            j.at("workDoneToken").get_to(workDoneToken);
        }
        if (j.contains("partialResultToken")) {
            j.at("partialResultToken").get_to(partialResultToken);
        }
    }
};

/**
 * 'textDocument/declaration' request
 */
struct DeclarationParams : WorkDoneProgressParams, TextDocumentPositionParams, PartialResultParams {
    void fromJson(const nlohmann::json &j) override {
        if (j.contains("textDocument")) {
            j.at("textDocument").at("uri").get_to(textDocument.uri);
        }
        if (j.contains("position")) {
            j.at("position").at("line").get_to(position.line);
            j.at("position").at("character").get_to(position.character);
        }
        if (j.contains("workDoneToken")) {
            j.at("workDoneToken").get_to(workDoneToken);
        }
        if (j.contains("partialResultToken")) {
            j.at("partialResultToken").get_to(partialResultToken);
        }
    }
};

/**
 * 'initialize' response
 */
struct InitializeResult : ResultBase {
    struct ServerCapabilities {
        int textDocumentSync = 1; // full sync
        bool definitionProvider = true;
        bool implementationProvider = true;
        bool documentFormattingProvider = true;
        bool documentRangeFormattingProvider = true;
    } capabilities;

    struct ServerInfo {
        std::string name = "ogre-scripts-LSP";
        // toDo (gonzalezext)[26.01.24]: version should be provide via CMAKE variable
        std::string version = "1.0.0";
    } serverInfo;

    nlohmann::json toJson() override {
        return nlohmann::json{
                {"capabilities", {
                                         {"textDocumentSync", capabilities.textDocumentSync},
                                         {"definitionProvider", capabilities.definitionProvider},
                                         {"implementationProvider", capabilities.implementationProvider},
                                         {"documentRangeFormattingProvider", capabilities.documentRangeFormattingProvider},
                                         {"documentFormattingProvider", capabilities.documentFormattingProvider}}
                },
                {"serverInfo",   {
                                         {"name",             serverInfo.name},
                                         {"version",            serverInfo.version}}
                }
        };
    };
};

struct RequestMessage : Message {
    std::string id;
    std::string method;
    ParamsBase *params;

    RequestMessage() : Message("") {
        jsonrpc = method = id = "";
        params = new ParamsBase();
    }

    void fromJson(const nlohmann::json &j) override {
        if (j.contains("jsonrpc")) {
            j.at("jsonrpc").get_to(jsonrpc);
        }
        if (j.contains("id")) {
            if (j.at("id").is_number()) {
                id = std::to_string(j.at("id").template get<int>());
            } else {
                j.at("id").get_to(id);
            }
        }
        if (j.contains("method")) {
            j.at("method").get_to(method);
        }
        if (j.contains("params")) {
            if ("initialize" == method) {
                params = new InitializeParams();
                params->fromJson(j.at("params"));
            } else if ("textDocument/formatting" == method) {
                params = new DocumentFormattingParams();
                params->fromJson(j.at("params"));
            } else if ("textDocument/rangeFormatting" == method) {
                params = new DocumentRangeFormattingParams();
                params->fromJson(j.at("params"));
            } else if ("textDocument/definition" == method) {
                params = new DefinitionParams();
                params->fromJson(j.at("params"));
            } else if ("textDocument/declaration" == method) {
                params = new DeclarationParams();
                params->fromJson(j.at("params"));
            }
        }
    }

    nlohmann::json toJson() override {
        return {};
    }
};

enum ErrorCode {
    parse_error = -32700,
    invalid_request = -32600,
    method_not_found = -32601,
    invalid_params = -32602,
    internal_error = -32603,
    server_not_initialized = -32002,
    server_cancelled = -32802,
    request_cancelled = -32800,
};

struct ResponseMessage : Message {
    std::string id;
    ResultBase *result;

    struct ResponseError {
        ErrorCode code{};
        std::string message;
        std::string data;

        nlohmann::json toJson() {
            return nlohmann::json{{"code",    code},
                                  {"message", message},
                                  {"data",    data}};
        };
    } error;

    ResponseMessage() : Message("2.0") {
        id = "";
        result = nullptr;
        error = {};
    }

    nlohmann::json toJson() override {
        if (nullptr != result) {
            return nlohmann::json{{"id",     id},
                                  {"result", result->toJson()}};
        } else {
            return nlohmann::json{{"id",    id},
                                  {"error", error.toJson()}};
        }
    };

    void fromJson(const nlohmann::json &j) override {}
};

struct Action {
    int contentLength;
    std::string contentType;
    Message *message;
};

#endif //OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H