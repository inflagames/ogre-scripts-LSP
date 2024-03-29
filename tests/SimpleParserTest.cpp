#include "gtest/gtest.h"

#include "../src/lang/parser.h"
#include "../src/lang/params_validator.h"

TEST (ParserTest, parse_ShouldParseFileCorrectly_example002) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example002.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example001) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example001.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example003) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example003.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example004) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example004.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example005) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example005.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example006) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example006.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example007) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example007.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example008) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example008.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_example009) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/valid_examples/example009.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/full_test_material.material";
    parser->loadScript(scriptFile);

    parser->parse();
    OgreScriptLSP::ParamsValidator::getSingleton()->paramsAnalysis(parser);

    ASSERT_EQ(0, parser->getExceptions()->size());
    ASSERT_EQ(7, parser->getScript()->programs.size());
    ASSERT_EQ(4, parser->getScript()->materials.size());
    ASSERT_EQ(4, parser->getScript()->abstracts.size());
    ASSERT_EQ(3, parser->getScript()->imports.size());
}

TEST (ParserTest, parse_ShouldParseFileCorrectly_withBadTokens) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/scanner/bad_tokens.material";
    parser->loadScript(scriptFile);

    parser->parse();

    ASSERT_EQ(1, parser->getExceptions()->size());
    ASSERT_EQ(0, parser->getScript()->programs.size());
    ASSERT_EQ(1, parser->getScript()->materials.size());
    ASSERT_EQ(0, parser->getScript()->abstracts.size());
    ASSERT_EQ(0, parser->getScript()->imports.size());
}

TEST (ParserTest, parse_ShouldCreateAstForFragmentDeclaration) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/scanner/frag_vert_declarations.material";
    parser->loadScript(scriptFile);

    parser->parse();

    OgreScriptLSP::MaterialScriptAst *s = parser->getScript();
    ASSERT_TRUE(s != nullptr);
    ASSERT_EQ(2, s->programs.size());

    OgreScriptLSP::ProgramAst *p = s->programs[0].get();
    ASSERT_EQ(2, p->highLevelProgramsType.size());
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(0, p->defaults.size());

    p = s->programs[1].get();
    ASSERT_EQ(1, p->highLevelProgramsType.size());
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(1, p->defaults.size());
}

TEST (ParserTest, parse_ShouldCreateAstForMaterialDeclaration) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/parser/material_simple_example.material";
    parser->loadScript(scriptFile);

    parser->parse();

    OgreScriptLSP::MaterialScriptAst *s = parser->getScript();
    ASSERT_TRUE(s != nullptr);
    ASSERT_EQ(2, s->materials.size());

    // Validate fist material
    OgreScriptLSP::MaterialAst *m = s->materials[0].get();
    ASSERT_EQ("Parent", m->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->name.tk);
    ASSERT_EQ("", m->parent.literal);
    ASSERT_EQ(0, m->params.size());
    ASSERT_EQ(1, m->techniques.size());

    OgreScriptLSP::TechniqueAst *t = m->techniques[0].get();
    ASSERT_EQ(1, t->passes.size());

    OgreScriptLSP::PassAst *p = t->passes[0].get();
    ASSERT_EQ("some", p->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, p->name.tk);
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(2, p->programsReferences.size());
    ASSERT_EQ(1, p->textures.size());

    OgreScriptLSP::MaterialProgramAst *pr = p->programsReferences[0].get();
    ASSERT_EQ("vertex_example", pr->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, pr->name.tk);
    ASSERT_EQ(2, pr->params.size());

    pr = p->programsReferences[1].get();
    ASSERT_EQ("fragment_example", pr->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, pr->name.tk);
    ASSERT_EQ(1, pr->params.size());

    OgreScriptLSP::TextureUnitAst *tu = p->textures[0].get();
    ASSERT_EQ("normal", tu->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, tu->name.tk);
    ASSERT_EQ(1, pr->params.size());

    // Validate second material
    m = s->materials[1].get();
    ASSERT_EQ("Child", m->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->name.tk);
    ASSERT_EQ("Parent", m->parent.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->parent.tk);
    ASSERT_EQ(1, m->params.size());
    ASSERT_EQ(1, m->techniques.size());

    t = m->techniques[0].get();
    ASSERT_EQ(1, t->passes.size());

    p = t->passes[0].get();
    ASSERT_EQ("some", p->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, p->name.tk);
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(0, p->programsReferences.size());
    ASSERT_EQ(1, p->textures.size());

    tu = p->textures[0].get();
    ASSERT_EQ("normal", tu->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, tu->name.tk);
    ASSERT_EQ(1, pr->params.size());
}
