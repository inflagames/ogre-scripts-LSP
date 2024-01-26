//
// Created by gonzalezext on 26.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H
#define OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H

#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

struct Message {
    std::string jsonrpc;

    explicit Message(std::string jsonrpc) : jsonrpc(std::move(jsonrpc)) {}

    virtual void fromJson(const nlohmann::json &j) = 0;

    virtual nlohmann::json toJson() = 0;
};


struct ParamsBase {
    virtual void fromJson(const nlohmann::json &j) {};
};

struct WorkDoneProgressParams : ParamsBase {
    std::string workDoneToken;
};

struct ResultBase {
    virtual nlohmann::json toJson() {};
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
    int processId;
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
    } capabilities;
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
    struct TextDocumentIdentifier {
        std::string uri;
    } textDocument;

    struct FormattingOptions {
        uint32_t tabSize;
        bool insertSpaces;
        bool trimTrailingWhitespace = true;
        bool insertFinalNewline = false;
        bool trimFinalNewLines = false;
    } options;

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
 * 'initialize' response
 */
struct InitializeResult : ResultBase {
    struct ServerCapabilities {
        bool documentFormattingProvider = true;
    } capabilities;

    struct ServerInfo {
        std::string name = "ogre-scripts-LSP";
        // toDo (gonzalezext)[26.01.24]: version should be provide via CMAKE variable
        std::string version = "1.0.0";
    } serverInfo;

    nlohmann::json toJson() override {
        return nlohmann::json{
                {"capabilities", {{"documentFormattingProvider", capabilities.documentFormattingProvider}}},
                {"serverInfo",   {{"name",                       serverInfo.name}, {"version", serverInfo.version}}}
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
            }
        }
    }

    nlohmann::json toJson() override {}
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
        ErrorCode code;
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

#endif //OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H
