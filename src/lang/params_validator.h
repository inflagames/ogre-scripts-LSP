#ifndef OGRE_SCRIPTS_LSP_LIB_PARAMS_VALIDATOR_H
#define OGRE_SCRIPTS_LSP_LIB_PARAMS_VALIDATOR_H


#include <utility>

#include "ast_tree.h"
#include "parser.h"

namespace OgreScriptLSP {
    class ParamsTree {
    public:
        ParamsTree *parent = nullptr;

        Token _token{};
        std::vector<std::string> tokenNames;
        std::vector<std::shared_ptr<ParamsTree>> children;

        bool isEndParam = false;

        ParamsTree() = default;

        explicit ParamsTree(ParamsTree *parent) {
            this->parent = parent;
        }

        ParamsTree *tokenName(std::string tokenName) {
            tokenNames.push_back(std::move(tokenName));
            return this;
        }

        bool checkNames(const std::string &tokenName) {
            return std::any_of(tokenNames.begin(), tokenNames.end(),
                               [tokenName](const std::string &n) { return n == tokenName; });
        }

        ParamsTree *newChildren(ParamsTree *node) {
            for (const auto &a: node->children) {
                children.push_back(a);
            }
            return this;
        }

        ParamsTree *newChild(const std::vector<std::string> &tokenNamesIns, Token token) {
            for (const auto &ch: children) {
                int count = 0;
                for (const auto &tk: tokenNamesIns) {
                    if (ch->checkNames(tk)) {
                        count++;
                    }
                }
                if (count == tokenNamesIns.size()) {
                    return ch.get();
                }
            }

            auto ch = newChild()->token(token);
            for (const auto &tk: tokenNamesIns) {
                ch->tokenName(tk);
            }
            return ch;
        }

        ParamsTree *newChild() {
            auto newChildPtr = new ParamsTree(this);
            children.emplace_back(std::shared_ptr<ParamsTree>(newChildPtr));
            return newChildPtr;
        }

        ParamsTree *endChild() const { // NOLINT(*-use-nodiscard)
            return parent;
        }

        ParamsTree *token(Token token) {
            _token = token;
            return this;
        }

        ParamsTree *markAsEnd() {
            isEndParam = true;
            return this;
        }

        void
        validateParams(OgreScriptLSP::ParamAst *paramAst, int position, std::string error) { // NOLINT(*-no-recursion)
            if (isEndParam && (children.empty() || paramAst->items.size() <= position)) {
                return;
            } else if (children.empty()) {
                // invalid structure
                throw ParamsException(std::move(error), paramAst->items[position - 1].toRange());
            }

            auto &param = paramAst->items[position];
            for (const auto &ch: children) {
                if (ch && ch->_token == param.tk) {
                    if ((param.tk == identifier && ch->checkNames(param.literal)) ||
                        (param.tk == identifier && ch->checkNames("(identifier)")) ||
                        (param.tk == match_literal && ch->checkNames("(match)")) ||
                        (param.tk == number_literal && ch->checkNames("(number)"))) {
                        return ch->validateParams(paramAst, position + 1, error);
                    }
                }
            }

            throw ParamsException(std::move(error), paramAst->items[position].toRange());
        }
    };

    class ParamsValidator {
    public:
        ParamsValidator();

        static ParamsValidator *getSingleton();

        void paramsAnalysis(Parser *parser);

        void paramsAnalysis(SamplerAst *, Parser *);

        void paramsAnalysis(MaterialAst *, Parser *);

        void paramsAnalysis(TechniqueAst *, Parser *);

        void paramsAnalysis(PassAst *, Parser *);

        void paramsAnalysis(TextureUnitAst *, Parser *);

        void validateParams(std::vector<std::unique_ptr<ParamAst>> *params, Parser *parser);

        void validateParam(OgreScriptLSP::ParamAst *paramAst);

        void setupSamplerParams();

        void setupMaterialParams();

        void setupTechniqueParams();

        void setupPassParams();

        void setupTextureUnitParams();

        std::vector<std::string> nextParamsToken(const std::string &definition, int &position);

        std::string nextParamToken(const std::string &definition, int &position, char delimiter = '>');

        void consumeSpaces(const std::string &definition, int &position);

        void loadChildFromDefinition(std::string &definition, ParamsTree *treeRoot);

        [[nodiscard]] ParamsTree *getPassParamTree() const;

    private:
        std::unique_ptr<ParamsTree> materialParamTree;
        std::unique_ptr<ParamsTree> techniqueParamTree;
        std::unique_ptr<ParamsTree> passParamTree;
        std::unique_ptr<ParamsTree> textureUnitParamTree;
        std::unique_ptr<ParamsTree> samplerParamTree;
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_PARAMS_VALIDATOR_H
