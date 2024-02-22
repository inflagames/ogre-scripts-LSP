#ifndef JS_AI_PARSER_H
#define JS_AI_PARSER_H

#include "scanner.h"
#include "ast_tree.h"
#include "exceptions.h"
#include "lsp_protocol.h"
#include "goto.h"
#include <memory>

#define MATERIAL_BLOCK 1
#define PROGRAM_VERTEX_BLOCK 2
#define PROGRAM_FRAGMENT_BLOCK 3
#define PROGRAM_GEOMETRY_BLOCK 4
#define PROGRAM_HULL_BLOCK 5
#define PROGRAM_DOMAIN_BLOCK 6
#define PROGRAM_COMPUTE_BLOCK 7
#define TECHNIQUE_BLOCK 8
#define PASS_BLOCK 9
#define TEXTURE_UNIT_BLOCK 10
#define RTSHADER_BLOCK 11
#define SHARED_PARAMS_BLOCK 12
#define TEXTURE_SOURCE_BLOCK 13

namespace OgreScriptLSP {
    class Parser {
    private:
        std::unique_ptr<Scanner> scanner;

        std::vector<TokenValue> tokens;
        int currentToken = 0;

        std::unique_ptr<MaterialScriptAst> script;
        std::map<std::pair<int, std::string>, TokenValue> declarations;

        std::vector<BaseException> exceptions;

    public:
        std::string uri;

        Parser();

        ~Parser() = default;

        MaterialScriptAst *getScript() { return script.get(); }

        [[nodiscard]] const Scanner *getScanner() const;

        [[nodiscard]] std::unique_ptr<std::map<std::pair<int, std::string>, TokenValue>> getDeclarations() const {
            auto decl = std::make_unique<std::map<std::pair<int, std::string>, TokenValue>>(declarations);
            return decl;
        }

        std::vector<BaseException> getExceptions() {
            return exceptions;
        }

        void loadScript(const std::string &fileUri, const std::string &code = "");

        static std::string uriToPath(const std::string &uri);

        void parse(const std::string &fileUri);

        void parse();

        // IMPORT STATEMENT
        void importBlock(MaterialScriptAst *scriptAst);

        // ABSTRACT STATEMENT
        void abstract(MaterialScriptAst *scriptAst);

        // SHARED PARAMS STATEMENT
        void sharedParams(MaterialScriptAst *scriptAst);

        void sharedParamsBody(SharedParamsAst *sharedParams);

        // PROGRAM STATEMENT
        void program(MaterialScriptAst *scriptAst);

        void programOpt(ProgramAst *program);

        void programBody(ProgramAst *program);

        void programDefaults(ProgramAst *program);

        // MATERIAL STATEMENT
        void material(MaterialScriptAst *scriptAst);

        void materialBody(MaterialAst *material);

        void materialTechnique(MaterialAst *material);

        void materialTechniqueBody(TechniqueAst *technique);

        void techniqueShadowMaterial(TechniqueAst *technique);

        void materialPass(TechniqueAst *technique);

        void materialPassBody(PassAst *pass);

        void materialTexture(PassAst *pass);

        void materialTextureBody(TextureUnitAst *texture);

        void materialRtShader(RtShaderAst *shader);

        void materialRtShaderBody(RtShaderAst *shader);

        void materialTextureSource(TextureUnitAst *texture);

        void materialTextureSourceBody(TextureSourceAst *shader);

        void materialProgramRef(PassAst *pass);

        void materialProgramRefBody(MaterialProgramAst *programRef);

        void programSharedParams(MaterialProgramSharedParamAst *sharedParam);

        // THIS IS COMMON SECTION
        void paramsLine(ParamAst *params);

        /**
         * current token
         */
        TokenValue getToken();

        void recuperateLine();

        static int getProgramBlockIdk(Token tk) {
            return tk == fragment_program_tk || tk == fragment_program_ref_tk ? PROGRAM_FRAGMENT_BLOCK :
                   tk == vertex_program_tk || tk == vertex_program_ref_tk ? PROGRAM_VERTEX_BLOCK :
                   tk == geometry_program_tk || tk == geometry_program_ref_tk ? PROGRAM_GEOMETRY_BLOCK :
                   tk == tessellation_hull_program_tk || tk == tessellation_hull_program_ref_tk ? PROGRAM_HULL_BLOCK :
                   tk == tessellation_domain_program_tk || tk == tessellation_domain_program_ref_tk
                   ? PROGRAM_DOMAIN_BLOCK :
                   PROGRAM_COMPUTE_BLOCK;
        }

        void nextTokenAndConsumeEndLines();

        void consumeOpenCurlyBracket();

        void consumeCloseCurlyBracket();

        void consumeToken(Token token, std::string errorMessage = "", bool consumeEndLines = false);

        void nextToken();

        void consumeEndLines();

        bool isEof();

        void initSwap();

        void registerDeclaration(AstObject *object, int type);

        bool isMainStructure();

        void
        objectDefinition(AstObject *astObject, std::string error1, std::string error2, bool notTopLevelObject = false);
    };
}


#endif //JS_AI_PARSER_H
