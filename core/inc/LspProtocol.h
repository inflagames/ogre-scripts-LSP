//
// Created by gonzalezext on 26.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H
#define OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H

#include <vector>

#include <nlohmann/json.hpp>

struct Message {
    std::string jsonrpc;

    virtual void formJson(const nlohmann::json &j) {};

    virtual void toJson(nlohmann::json &j) {};
};


struct ParamsBase {
    virtual void formJson(const nlohmann::json &j) {};
};

struct WorkDoneProgressParams : ParamsBase {
    std::string workDoneToken;
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

    virtual void formJson(const nlohmann::json &j) {
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
 * 'initialize' response
 */
struct InitializeResult {
    struct ServerCapabilities {
        bool documentFormattingProvider;
    } capabilities;

    struct ServerInfo {
        std::string name;
        std::string version;
    } serverInfo;
};

struct RequestMessage : Message {
    std::string id;
    std::string method;
    ParamsBase *params;

    void fromJson(const nlohmann::json &j) {
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
                params->formJson(j.at("params"));
            }
        }
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
    std::string result;
    struct ResponseError {
        ErrorCode code;
        std::string message;
        std::string data;
    } error;

    virtual void toJson(nlohmann::json &j) {
        j = nlohmann::json{{"id",     id},
                           {"result", result}};
    };
};

struct Action {
    int contentLength;
    std::string contentType;
    Message *message;
};

#endif //OGRE_SCRIPTS_LSP_LIB_LSPPROTOCOL_H
