#include "gtest/gtest.h"

#include "../src/lang/parser.h"
#include "../src/lang/params_validator.h"

TEST (ValidParamsTest, params_shouldValidatePassParams) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/params/valid_params_pass_block.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ValidParamsTest, params_shouldValidateMaterialParams) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/params/valid_params_material_block.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ValidParamsTest, params_shouldValidateTechniqueParams) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/params/valid_params_technique_block.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ValidParamsTest, params_shouldValidateTextureUnitParams) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/params/valid_params_texture_unit_block.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ValidParamsTest, params_shouldValidateSampleParams) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/params/valid_params_sample_block.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}


void validateChild(OgreScriptLSP::ParamsTree *&tree,
                   const std::vector<std::string> &tokenNames,
                   OgreScriptLSP::Token token,
                   bool isFinal = false) {
    for (const auto &ch: tree->children) {
        int count = 0;
        for (const auto &tkn: tokenNames) {
            if (ch->_token == token && ch->checkNames(tkn)) {
                count++;
            }
        }
        if (count == tokenNames.size()) {
            tree = ch.get();
            ASSERT_EQ(isFinal, tree->isEndParam);
            return;
        }
    }
    ASSERT_TRUE(false);
}

TEST (ValidParamsTest, paramsStructure_shouldExtractNextSimpleToken) {
    std::string def = "<example> <some><kk>";
    OgreScriptLSP::ParamsTree *tree = OgreScriptLSP::ParamsValidator::getSingleton()->getPassParamTree();
    OgreScriptLSP::ParamsValidator::getSingleton()->loadChildFromDefinition(def, tree);
    OgreScriptLSP::ParamsTree *ch = OgreScriptLSP::ParamsValidator::getSingleton()->getPassParamTree();

    validateChild(ch, {"example"}, OgreScriptLSP::identifier);
    validateChild(ch, {"some"}, OgreScriptLSP::identifier);
    validateChild(ch, {"kk"}, OgreScriptLSP::identifier, true);
}

TEST (ValidParamsTest, paramsStructure_shouldExtractNextMultiplesTokens) {
    std::string def = "<example> [ <some> <kk> ][<asv><hgkd>](number)";
    OgreScriptLSP::ParamsTree *tree = OgreScriptLSP::ParamsValidator::getSingleton()->getPassParamTree();
    OgreScriptLSP::ParamsValidator::getSingleton()->loadChildFromDefinition(def, tree);
    OgreScriptLSP::ParamsTree *ch = OgreScriptLSP::ParamsValidator::getSingleton()->getPassParamTree();

    validateChild(ch, {"example"}, OgreScriptLSP::identifier);
    validateChild(ch, {"some", "kk"}, OgreScriptLSP::identifier);
    validateChild(ch, {"asv", "hgkd"}, OgreScriptLSP::identifier);
    validateChild(ch, {"(number)"}, OgreScriptLSP::number_literal, true);
}
