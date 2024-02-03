//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/parser.h"

TEST (ParserTest, parse_ShouldCreateAstForFragmentDeclaration) {
    auto *parser = new OgreScriptLSP::Parser();
    std::string scriptFile = "./examples/scanner/frag_vert_declarations.material";
    parser->loadScript(scriptFile);

    parser->parse();

    OgreScriptLSP::MaterialScriptAst *s = parser->getScript();
    ASSERT_TRUE(s != nullptr);
    ASSERT_EQ(2, s->programs.size());

    OgreScriptLSP::ProgramAst *p = s->programs[0];
    ASSERT_EQ(2, p->highLevelProgramsType.size());
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(0, p->defaults.size());

    p = s->programs[1];
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
    OgreScriptLSP::MaterialAst *m = s->materials[0];
    ASSERT_EQ("Parent", m->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->name.tk);
    ASSERT_EQ("", m->parent.literal);
    ASSERT_EQ(0, m->params.size());
    ASSERT_EQ(1, m->techniques.size());

    OgreScriptLSP::TechniqueAst *t = m->techniques[0];
    ASSERT_EQ(1, t->passes.size());

    OgreScriptLSP::PassAst *p = t->passes[0];
    ASSERT_EQ("some", p->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, p->name.tk);
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(2, p->programsReferences.size());
    ASSERT_EQ(1, p->textures.size());

    OgreScriptLSP::MaterialProgramAst *pr = p->programsReferences[0];
    ASSERT_EQ("vertex_example", pr->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, pr->name.tk);
    ASSERT_EQ(2, pr->params.size());

    pr = p->programsReferences[1];
    ASSERT_EQ("fragment_example", pr->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, pr->name.tk);
    ASSERT_EQ(1, pr->params.size());

    OgreScriptLSP::TextureUnitAst *tu = p->textures[0];
    ASSERT_EQ("normal", tu->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, tu->name.tk);
    ASSERT_EQ(1, pr->params.size());

    // Validate second material
    m = s->materials[1];
    ASSERT_EQ("Child", m->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->name.tk);
    ASSERT_EQ("Parent", m->parent.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, m->parent.tk);
    ASSERT_EQ(1, m->params.size());
    ASSERT_EQ(1, m->techniques.size());

    t = m->techniques[0];
    ASSERT_EQ(1, t->passes.size());

    p = t->passes[0];
    ASSERT_EQ("some", p->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, p->name.tk);
    ASSERT_EQ(1, p->params.size());
    ASSERT_EQ(0, p->programsReferences.size());
    ASSERT_EQ(1, p->textures.size());

    tu = p->textures[0];
    ASSERT_EQ("normal", tu->name.literal);
    ASSERT_EQ(OgreScriptLSP::identifier, tu->name.tk);
    ASSERT_EQ(1, pr->params.size());
}
