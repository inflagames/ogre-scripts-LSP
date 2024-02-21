#ifndef OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H
#define OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H

#include <iostream>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"
#include "logs.h"

#define DIAGNOSTIC_SEVERITY_ERROR 1
#define DIAGNOSTIC_SEVERITY_WARNING 2
#define DIAGNOSTIC_SEVERITY_INFORMATION 3
#define DIAGNOSTIC_SEVERITY_HINT 4

#define NOTIFICATION_PUBLISH_DIAGNOSTICS "textDocument/publishDiagnostics"

namespace OgreScriptLSP {
    struct Message {
        std::string jsonrpc;

        explicit Message(std::string jsonrpc) : jsonrpc(std::move(jsonrpc)) {}

        virtual void fromJson(const nlohmann::json &j) = 0;

        virtual nlohmann::json toJson() = 0;
    };

    struct TextDocumentIdentifier {
        std::string uri;
    };

    struct VersionedTextDocumentIdentifier : TextDocumentIdentifier {
        int version = 0;
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

        void fromJson(const nlohmann::json &j) {
            if (j.contains("line")) {
                j.at("line").get_to(line);
            }
            if (j.contains("character")) {
                j.at("character").get_to(character);
            }
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

        nlohmann::json toJson() {
            return nlohmann::json{
                    {"start", {{"line", start.line}, {"character", start.character}}},
                    {"end",   {{"line", end.line},   {"character", end.character}}}
            };
        }

        void fromJson(const nlohmann::json &j) {
            if (j.contains("start")) {
                start.fromJson(j.at("start"));
            }
            if (j.contains("end")) {
                end.fromJson(j.at("end"));
            }
        }

        static Range toRange(int line, int character, int size) {
            return {{line, character},
                    {line, character + size}};
        }
    };

    struct TextDocumentContentChangeEvent {
        Range range{}; // optional
        std::string text;

        void fromJson(const nlohmann::json &j) {
            if (j.contains("text")) {
                j.at("text").get_to(text);
            }
            if (j.contains("range")) {
                range.fromJson(j.at("range"));
            }
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

    struct TextDocumentItem {
        std::string uri;
        std::string languageId;
        int version{};
        std::string text;

        void fromJson(const nlohmann::json &j) {
            if (j.contains("uri")) {
                j.at("uri").get_to(uri);
            }
            if (j.contains("languageId")) {
                j.at("languageId").get_to(languageId);
            }
            if (j.contains("version")) {
                j.at("version").get_to(version);
            }
            if (j.contains("text")) {
                j.at("text").get_to(text);
            }
        }
    };

    struct ResultArray : ResultBase {
        std::vector<ResultBase *> elements;

        nlohmann::json toJson() override {
            nlohmann::json array = nlohmann::json::array();
            for (auto ele: elements) {
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
                    {"range", range.toJson()}
            };
        }
    };

    enum SymbolKind {
        File = 1, Module, Namespace, Package, Class, Method, Property, Field, Constructor, Enum, Interface, Function,
        Variable, Constant, String, Number, Boolean, Array, Object, Key, Null, EnumMember, Struct, Event, Operator,
        TypeParameter
    };

    struct DocumentSymbol : ResultBase {
        std::string name;
        SymbolKind kind;
        Range range;
        Range selectionRange;
        std::vector<DocumentSymbol> children;

        DocumentSymbol(std::string name, SymbolKind kind, Range range, Range selectionRange) : name(std::move(name)),
                                                                                               kind(kind),
                                                                                               range(range),
                                                                                               selectionRange(
                                                                                                       selectionRange) {}

        nlohmann::json toJson() override {
            return nlohmann::json{
                    {"name", name},
                    {"kind", kind},
                    {"range", range.toJson()},
                    {"selectionRange", selectionRange.toJson()},
//                    {"children": childrenArr}
            };
        }
    };

    struct TextEdit : ResultBase {
        Range range;
        std::string newText;

        TextEdit(Position start, Position end, std::string newText) : range({start, end}),
                                                                      newText(std::move(newText)) {}

        nlohmann::json toJson() override {
            return nlohmann::json{
                    {"range",   range.toJson()},
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

    struct PublishDiagnosticsClientCapabilities {
        /**
         * Client supports the tag property to provide meta data about a diagnostic.
         * Clients supporting tags have to handle unknown tags gracefully.
         *
         * @since 3.15.0
         */
        struct TagSupport {
            /**
             * 1 --> for unnecessary/unused code<br>
             * 2 --> for deprecated or obsolete code
             */
            std::vector<uint8_t> valueSet;
        } tagSupport;

        /**
         * Whether the client interprets the version property of the
         * `textDocument/publishDiagnostics` notification's parameter.
         *
         * @since 3.15.0
         */
        bool versionSupport = false;
    };

    struct CompletionClientCapabilities {
    };

    struct TextDocumentClientCapabilities {
        PublishDiagnosticsClientCapabilities publishDiagnostics;
        CompletionClientCapabilities completion;
    };

    struct WorkspaceFolder {
        std::string uri;
        std::string name;
    };

    struct ClientCapabilities {
        bool workspaceFolders{};
        TextDocumentClientCapabilities textDocument;
    };

    /**
     * 'initialize'
     */
    struct InitializeParams : WorkDoneProgressParams {
        int processId{};
        std::string rootPath; // probably not used
        std::string rootUri; // probably not used
        ClientCapabilities capabilities;
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
            if (j.contains("capabilities")) {
                if (j.at("capabilities").contains("textDocument")) {
                    if (j.at("capabilities").at("textDocument").contains("publishDiagnostics")) {
                        if (j.at("capabilities").at("textDocument").at("publishDiagnostics").contains(
                                "versionSupport")) {
                            j.at("capabilities").at("textDocument").at("publishDiagnostics").at("versionSupport")
                                    .get_to(capabilities.textDocument.publishDiagnostics.versionSupport);
                        }
                        if (j.at("capabilities").at("textDocument").at("publishDiagnostics").contains("tagSupport")) {
                            for (const auto &t: j.at("capabilities").at("textDocument").at("publishDiagnostics").at(
                                    "tagSupport").at("valueSet")) {
                                capabilities.textDocument.publishDiagnostics.tagSupport.valueSet.push_back(
                                        t.template get<uint8_t>());
                            }
                        }
                    }
                }
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
     * 'textDocument/semanticTokens/full'
     */
    struct SemanticTokensParams : PartialResultParams, WorkDoneProgressParams {
        TextDocumentIdentifier textDocument;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
        }
    };

    /**
     * 'textDocument/semanticTokens/full/delta'
     */
    struct SemanticTokensDeltaParams : PartialResultParams, WorkDoneProgressParams {
        TextDocumentIdentifier textDocument;
        std::string previousResultId;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
        }
    };

    /**
     * 'textDocument/semanticTokens/range'
     */
    struct SemanticTokensRangeParams : PartialResultParams, WorkDoneProgressParams {
        TextDocumentIdentifier textDocument;
        Range range{};

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
            if (j.contains("range")) {
                range.fromJson(j.at("range"));
            }
        }
    };

    /**
     * 'textDocument/documentSymbol'
     */
    struct DocumentSymbolParams : PartialResultParams, WorkDoneProgressParams {
        TextDocumentIdentifier textDocument;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
        }
    };

    /**
     * 'textDocument/didOpen'
     */
    struct DidOpenTextDocumentParams : ParamsBase {
        TextDocumentItem textDocument;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument")) {
                textDocument.fromJson(j.at("textDocument"));
            }
        }
    };

    /**
     * 'textDocument/didClose'
     */
    struct DidCloseTextDocumentParams : ParamsBase {
        TextDocumentIdentifier textDocument;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
        }
    };

    /**
     * 'textDocument/didSave'
     */
    struct DidSaveTextDocumentParams : ParamsBase {
        TextDocumentIdentifier textDocument;
        std::string text;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
            if (j.contains("text")) {
                j.at("text").get_to(text);
            }
        }
    };

    /**
     * 'textDocument/didChange'
     */
    struct DidChangeTextDocumentParams : ParamsBase {
        VersionedTextDocumentIdentifier textDocument;
        std::vector<TextDocumentContentChangeEvent> contentChanges;

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("textDocument") && j.at("textDocument").contains("uri")) {
                j.at("textDocument").at("uri").get_to(textDocument.uri);
            }
            if (j.contains("textDocument") && j.at("textDocument").contains("version")) {
                j.at("textDocument").at("version").get_to(textDocument.version);
            }
            if (j.contains("contentChanges") && j.at("contentChanges").is_array()) {
                for (const auto &it: j.at("contentChanges")) {
                    TextDocumentContentChangeEvent nv;
                    nv.fromJson(it);
                    contentChanges.push_back(nv);
                }
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
            bool implementationProvider = false;
            bool documentFormattingProvider = true;
            bool documentRangeFormattingProvider = true;
            bool documentSymbolProvider = false;

            struct SemanticTokensOptions {
                struct SemanticTokensLegend {
                    std::vector<std::string> tokenTypes;
                    std::vector<std::string> tokenModifiers;

                    nlohmann::json toJson() {
                        nlohmann::json arrayTokenTypes = nlohmann::json::array();
                        nlohmann::json arrayTokenModifiers = nlohmann::json::array();
                        for (const auto &v: tokenTypes) {
                            arrayTokenTypes.push_back(v);
                        }
                        for (const auto &v: tokenModifiers) {
                            arrayTokenModifiers.push_back(v);
                        }
                        return nlohmann::json{
                                {"tokenTypes",     arrayTokenTypes},
                                {"tokenModifiers", arrayTokenModifiers},
                        };
                    }
                } legend;

                bool workDoneProgress = false; // WorkDoneProgressOptions
                bool range = true;
                bool full = true;

                nlohmann::json toJson() {
                    return nlohmann::json{
                            {"legend",           legend.toJson()},
                            {"workDoneProgress", workDoneProgress},
                            {"range",            range},
                            {"full",             full},
                    };
                }
            } semanticTokensProvider;
        } capabilities;

        struct ServerInfo {
            std::string name = "ogre-scripts-LSP";
            std::string version = APP_VERSION;
        } serverInfo;

        nlohmann::json toJson() override {
            return nlohmann::json{
                    {"capabilities", {
                                             {"textDocumentSync", capabilities.textDocumentSync},
                                             {"definitionProvider", capabilities.definitionProvider},
                                             {"implementationProvider", capabilities.implementationProvider},
                                             {"documentRangeFormattingProvider", capabilities.documentRangeFormattingProvider},
                                             {"documentSymbolProvider", capabilities.documentSymbolProvider},
                                             {"semanticTokensProvider", capabilities.semanticTokensProvider.toJson()},
                                             {"documentFormattingProvider", capabilities.documentFormattingProvider}}
                    },
                    {"serverInfo",   {
                                             {"name",             serverInfo.name},
                                             {"version",            serverInfo.version}}
                    }
            };
        };
    };

    struct Diagnostic {
        /**
         * 1: Error<br>
         * 2: Warning<br>
         * 3: Information<br>
         * 4: Hint
         */
        uint8_t severity;
        Range range;
        std::string message;
    };

    /**
     * 'textDocument/publishDiagnostics' response
     */
    struct PublishDiagnosticsParams : ResultBase {
        std::string uri;
        std::vector<Diagnostic> diagnostics;

        nlohmann::json toJson() override {
            auto diagnosticsArray = nlohmann::json::array();
            for (auto diag: diagnostics) {
                diagnosticsArray.push_back(nlohmann::json{
                        {"severity", diag.severity},
                        {"range",    diag.range.toJson()},
                        {"message",  diag.message}
                });
            }
            return nlohmann::json{{"uri",         uri},
                                  {"diagnostics", diagnosticsArray}};
        }
    };

    struct RequestMessage : Message {
        std::string id;
        std::string method;
        ParamsBase *params = nullptr;

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
                } else if ("textDocument/formatting" == method) {
                    params = new DocumentFormattingParams();
                } else if ("textDocument/rangeFormatting" == method) {
                    params = new DocumentRangeFormattingParams();
                } else if ("textDocument/definition" == method) {
                    params = new DefinitionParams();
                } else if ("textDocument/declaration" == method) {
                    params = new DeclarationParams();
                } else if ("textDocument/didOpen" == method) {
                    params = new DidOpenTextDocumentParams();
                } else if ("textDocument/didClose" == method) {
                    params = new DidCloseTextDocumentParams();
                } else if ("textDocument/didSave" == method) {
                    params = new DidSaveTextDocumentParams();
                } else if ("textDocument/documentSymbol" == method) {
                    params = new DocumentSymbolParams();
                } else if ("textDocument/didChange" == method) {
                    params = new DidChangeTextDocumentParams();
                }
                if (params != nullptr) {
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

    struct NotificationMessage : Message {
        std::string method;
        ResultBase *params;

        explicit NotificationMessage(std::string method) : Message("2.0") {
            this->method = std::move(method);
            params = nullptr;
        }

        void fromJson(const nlohmann::json &j) override {}

        nlohmann::json toJson() override {
            auto j = nlohmann::json{{"method", method}};

            if (params != nullptr) {
                j.emplace("params", params->toJson());
            }

            return j;
        }
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
}

#endif //OGRE_SCRIPTS_LSP_LIB_LSP_PROTOCOL_H
