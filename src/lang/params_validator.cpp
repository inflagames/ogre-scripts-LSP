#include "params_validator.h"


OgreScriptLSP::ParamsValidator::ParamsValidator() {
    setupMaterialParams();
    setupTechniqueParams();
    setupPassParams();
    setupTextureUnitParams();
    setupSamplerParams();
}

OgreScriptLSP::ParamsValidator *OgreScriptLSP::ParamsValidator::getSingleton() {
    static auto instance = new ParamsValidator();
    return instance;
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(Parser *parser) {
    for (auto &abs: parser->getScript()->abstracts) {
        switch (abs->type.tk) {
            case material_tk:
                paramsAnalysis(dynamic_cast<MaterialAst *>(abs->body.get()), parser);
                break;
            case technique_tk:
                paramsAnalysis(dynamic_cast<TechniqueAst *>(abs->body.get()), parser);
                break;
            case pass_tk:
                paramsAnalysis(dynamic_cast<PassAst *>(abs->body.get()), parser);
                break;
            case texture_unit_tk:
                paramsAnalysis(dynamic_cast<TextureUnitAst *>(abs->body.get()), parser);
                break;
            default:
                break;
        }
    }
    for (auto &mat: parser->getScript()->materials) {
        paramsAnalysis(mat.get(), parser);
    }

    for (auto &s: parser->getScript()->samplers) {
        paramsAnalysis(s.get(), parser);
    }
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(OgreScriptLSP::SamplerAst *sampler, Parser *parser) {
    validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&sampler->params), parser);
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(OgreScriptLSP::MaterialAst *mat, Parser *parser) {
    validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&mat->params), parser);

    for (auto &t: mat->techniques) {
        paramsAnalysis(t.get(), parser);
    }
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(OgreScriptLSP::TechniqueAst *t, Parser *parser) {
    validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&t->params), parser);

    for (auto &p: t->passes) {
        paramsAnalysis(p.get(), parser);
    }
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(OgreScriptLSP::PassAst *p, Parser *parser) {
    validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&p->params), parser);

    for (auto &s: p->shaders) {
        validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&s->params), parser);
    }
    for (auto &s: p->programsReferences) {
        validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&s->params), parser);
    }
    for (auto &s: p->textures) {
        paramsAnalysis(s.get(), parser);
    }
}

void OgreScriptLSP::ParamsValidator::paramsAnalysis(OgreScriptLSP::TextureUnitAst *p, Parser *parser) {
    validateParams(reinterpret_cast<std::vector<std::unique_ptr<ParamAst>> *>(&p->params), parser);
}

void OgreScriptLSP::ParamsValidator::validateParams(std::vector<std::unique_ptr<ParamAst>> *params, Parser *parser) {
    for (auto &p: *params) {
        try {
            validateParam(p.get());
        } catch (ParamsException &e) {
            parser->getExceptions()->push_back(e);
        }
    }
}

void OgreScriptLSP::ParamsValidator::validateParam(OgreScriptLSP::ParamAst *paramAst) {
    if (auto paramsPass = dynamic_cast<PassParamAst *>(paramAst)) {
        passParamTree->validateParams(paramsPass, 0, PARAM_PASS_ERROR);
    } else if (auto paramsMat = dynamic_cast<MaterialParamAst *>(paramAst)) {
        materialParamTree->validateParams(paramsMat, 0, PARAM_MATERIAL_ERROR);
    } else if (auto paramsTech = dynamic_cast<TechniqueParamAst *>(paramAst)) {
        techniqueParamTree->validateParams(paramsTech, 0, PARAM_TECHNIQUE_ERROR);
    } else if (auto paramsTextUnit = dynamic_cast<TextureUnitParamAst *>(paramAst)) {
        textureUnitParamTree->validateParams(paramsTextUnit, 0, PARAM_TEXTURE_UNIT_ERROR);
    } else if (auto paramsSampler = dynamic_cast<SamplerParamAst *>(paramAst)) {
        samplerParamTree->validateParams(paramsSampler, 0, PARAM_SAMPLER_ERROR);
    }
}

void OgreScriptLSP::ParamsValidator::setupSamplerParams() {
    samplerParamTree = std::make_unique<ParamsTree>();

    // filtering
    std::string filtering = "<filtering>[<none><bilinear><trilinear><anisotropic>]";
    loadChildFromDefinition(filtering, samplerParamTree.get());

    // max_anisotropy
    std::string maxAnisotropy = "<max_anisotropy>(number)";
    loadChildFromDefinition(maxAnisotropy, samplerParamTree.get());

    // mipmap_bias
    std::string mipmapBias = "<mipmap_bias>(number)";
    loadChildFromDefinition(mipmapBias, samplerParamTree.get());

    // compare_test
    std::string onOff = "[<on><off>]";
    std::string compareTest = "<compare_test>" + onOff;
    loadChildFromDefinition(compareTest, samplerParamTree.get());

    // tex_address_mode
    std::string texAddressModeOp = "[<wrap><mirror><clamp><border>]";
    std::string texAddressMode = "<tex_address_mode>" + texAddressModeOp;
    loadChildFromDefinition(texAddressMode, samplerParamTree.get());
    texAddressMode = "<tex_address_mode>" + texAddressModeOp + texAddressModeOp;
    loadChildFromDefinition(texAddressMode, samplerParamTree.get());
    texAddressMode = "<tex_address_mode>" + texAddressModeOp + texAddressModeOp + texAddressModeOp;
    loadChildFromDefinition(texAddressMode, samplerParamTree.get());

    // tex_border_colour
    std::string texBorderColour = "<tex_border_colour>(number)(number)(number)";
    loadChildFromDefinition(texBorderColour, samplerParamTree.get());
    texBorderColour = "<tex_border_colour>(number)(number)(number)(number)";
    loadChildFromDefinition(texBorderColour, samplerParamTree.get());

    // comp_func
    std::string compareFunctions = "[<always_fail><always_pass><less><less_equal><equal><not_equal><greater_equal><greater>]";
    std::string compFunc = "<comp_func>" + compareFunctions;
    loadChildFromDefinition(compFunc, samplerParamTree.get());
}

void OgreScriptLSP::ParamsValidator::setupMaterialParams() {
    materialParamTree = std::make_unique<ParamsTree>();

    // lod_strategy
    std::string lodStrategy = "<lod_strategy>[<distance_sphere><distance_box><pixel_count><screen_ratio_pixel_count>]";
    loadChildFromDefinition(lodStrategy, materialParamTree.get());

    // lod_values
    std::string lodValues = "<lod_values>";
    for (int i = 0; i < 20; ++i) {
        lodValues += "(number)";
        loadChildFromDefinition(lodValues, materialParamTree.get());
    }

    // lod_distances (deprecated)

    // receive_shadows
    std::string onOff = "[<on><off>]";
    std::string receiveShadows = "<receive_shadows>" + onOff;
    loadChildFromDefinition(receiveShadows, materialParamTree.get());

    // transparency_casts_shadows
    std::string transparencyCastsShadows = "<transparency_casts_shadows>" + onOff;
    loadChildFromDefinition(transparencyCastsShadows, materialParamTree.get());

    // set_texture_alias
    std::string setTextureAlias = "<set_texture_alias>(identifier)(identifier)";
    loadChildFromDefinition(setTextureAlias, materialParamTree.get());
}

void OgreScriptLSP::ParamsValidator::setupTechniqueParams() {
    techniqueParamTree = std::make_unique<ParamsTree>();

    // scheme
    std::string scheme = "<scheme>";
    loadChildFromDefinition(scheme, techniqueParamTree.get());

    // lod_index
    std::string lodIndex = "<lod_index>(number)";
    loadChildFromDefinition(lodIndex, techniqueParamTree.get());

    // gpu_vendor_rule | gpu_device_rule
    std::string gpuRule = "[<gpu_vendor_rule><gpu_device_rule>][<include><exclude>](identifier)";
    loadChildFromDefinition(gpuRule, techniqueParamTree.get());
    gpuRule = "[<gpu_vendor_rule><gpu_device_rule>][<include><exclude>](match)";
    loadChildFromDefinition(gpuRule, techniqueParamTree.get());
}

void OgreScriptLSP::ParamsValidator::setupPassParams() {
    passParamTree = std::make_unique<ParamsTree>();

    // ambient
    std::string colour = "(number)(number)(number)";
    std::string colourWithAlpha = colour + "(number)";
    std::string ambient = "<ambient>" + colour;
    loadChildFromDefinition(ambient, passParamTree.get());
    ambient = "<ambient>" + colourWithAlpha;
    loadChildFromDefinition(ambient, passParamTree.get());

    // diffuse
    std::string diffuse = "<diffuse>" + colour;
    loadChildFromDefinition(diffuse, passParamTree.get());
    diffuse = "<diffuse>" + colourWithAlpha;
    loadChildFromDefinition(diffuse, passParamTree.get());

    // specular
    std::string specular = "<specular>" + colour;
    loadChildFromDefinition(specular, passParamTree.get());
    specular = "<specular>" + colourWithAlpha;
    loadChildFromDefinition(specular, passParamTree.get());

    // emissive
    std::string emissive = "<emissive>" + colour;
    loadChildFromDefinition(emissive, passParamTree.get());
    emissive = "<emissive>" + colourWithAlpha;
    loadChildFromDefinition(emissive, passParamTree.get());

    // scene_blend
    std::string sceneBlendFactor = "[<one><zero><dest_colour><source_colour><one_minus_dest_colour><one_minus_source_colour><dest_alpha><source_alpha><one_minus_dest_alpha><one_minus_source_alpha>]";
    std::string sceneBlendType = "[<add><modulate><colour_blend><alpha_blend>]";
    std::string sceneBlend = "<scene_blend>" + sceneBlendType;
    loadChildFromDefinition(sceneBlend, passParamTree.get());
    sceneBlend = "<scene_blend>" + sceneBlendFactor + sceneBlendFactor;
    loadChildFromDefinition(sceneBlend, passParamTree.get());

    // separate_scene_blend
    std::string separateSceneBlend1 = "<separate_scene_blend>" + sceneBlendType + sceneBlendType;
    loadChildFromDefinition(separateSceneBlend1, passParamTree.get());
    std::string separateSceneBlend2 =
            "<separate_scene_blend>" + sceneBlendFactor + sceneBlendFactor + sceneBlendFactor + sceneBlendFactor;
    loadChildFromDefinition(separateSceneBlend2, passParamTree.get());

    // scene_blend_op
    std::string op = "[<add><subtract><reverse_subtract><min><max>]";
    std::string sceneBlendOp = "<scene_blend_op>" + op;
    loadChildFromDefinition(sceneBlendOp, passParamTree.get());

    // separate_scene_blend_op
    std::string separateSceneBlendOp = "<separate_scene_blend_op>" + op + op;
    loadChildFromDefinition(separateSceneBlendOp, passParamTree.get());

    // depth_check
    std::string onOff = "[<on><off>]";
    std::string depthCheck = "<depth_check>" + onOff;
    loadChildFromDefinition(depthCheck, passParamTree.get());

    // depth_write
    std::string depthWrite = "<depth_write>" + onOff;
    loadChildFromDefinition(depthWrite, passParamTree.get());

    // depth_func
    std::string func = "[<always_fail><always_pass><less><less_equal><equal><not_equal><greater_equal><greater>]";
    std::string depthFunc = "<depth_func>" + func;
    loadChildFromDefinition(depthFunc, passParamTree.get());

    // depth_bias
    std::string depthBias = "<depth_bias>";
    loadChildFromDefinition(depthBias, passParamTree.get());

    // iteration_depth_bias
    std::string iterationDepthBias = "<iteration_depth_bias>";
    loadChildFromDefinition(iterationDepthBias, passParamTree.get());

    // alpha_rejection
    std::string alphaRejection = "<alpha_rejection>" + func;
    loadChildFromDefinition(alphaRejection, passParamTree.get());

    // alpha_to_coverage
    std::string alphaToCoverage = "<alpha_to_coverage>" + onOff;
    loadChildFromDefinition(alphaToCoverage, passParamTree.get());

    // lightScissor
    std::string lightScissor = "<light_scissor>" + onOff;
    loadChildFromDefinition(lightScissor, passParamTree.get());

    // light_clip_planes
    std::string lightClipPlanes = "<light_clip_planes>" + onOff;
    loadChildFromDefinition(lightClipPlanes, passParamTree.get());

    // illumination_stage
    std::string state = "[<ambient><per_light><decal><none>]";
    std::string illuminationStage = "<illumination_stage>" + state;
    loadChildFromDefinition(illuminationStage, passParamTree.get());

    // transparent_sorting
    std::string onOffForce = "[<on><off><force>]";
    std::string transparentSorting = "<transparent_sorting>" + onOffForce;
    loadChildFromDefinition(transparentSorting, passParamTree.get());

    // cull_hardware
    std::string cullHardwareMode = "[<clockwise><anticlockwise><none>]";
    std::string cullHardware = "<cull_hardware>" + cullHardwareMode;
    loadChildFromDefinition(cullHardware, passParamTree.get());

    // cull_software
    std::string cullSoftwareMode = "[<back><front><none>]";
    std::string cullSoftware = "<cull_software>" + cullSoftwareMode;
    loadChildFromDefinition(cullSoftware, passParamTree.get());

    // lighting
    std::string lighting = "<lighting>" + onOff;
    loadChildFromDefinition(lighting, passParamTree.get());

    // shading
    std::string shadingModes = "[<flat><gouraud><phong>]";
    std::string shading = "<shading>" + shadingModes;
    loadChildFromDefinition(shading, passParamTree.get());

    // polygon_mode
    std::string polygonModeType = "[<solid><wireframe><points>]";
    std::string polygonMode = "<polygon_mode>" + polygonModeType;
    loadChildFromDefinition(polygonMode, passParamTree.get());

    // polygon_mode_overrideable
    std::string polygonModeOverrideable = "<polygon_mode_overrideable>[<true><false>]";
    loadChildFromDefinition(polygonModeOverrideable, passParamTree.get());

    // fog_override
    std::string fogOverrideType = "[<none><linear><exp><exp2>]";
    std::string fogOverride = "<fog_override><false>";
    loadChildFromDefinition(fogOverride, passParamTree.get());
    fogOverride = "<fog_override><true>" + fogOverrideType + colour + "(number)(number)(number)";
    loadChildFromDefinition(fogOverride, passParamTree.get());

    // colour_write
    std::string colourWrite = "<colour_write>" + onOff;
    loadChildFromDefinition(colourWrite, passParamTree.get());
    colourWrite = "<colour_write>" + onOff + onOff + onOff + onOff;
    loadChildFromDefinition(colourWrite, passParamTree.get());

    // start_light
    std::string startLight = "<start_light>(number)";
    loadChildFromDefinition(startLight, passParamTree.get());

    // max_lights
    std::string maxLights = "<max_lights>(number)";
    loadChildFromDefinition(maxLights, passParamTree.get());

    // iteration
    std::string lightType = "[<point><directional><spot>]";
    std::string iteration = "<iteration>(number)";
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>(number)<per_light>";
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>(number)<per_light>" + lightType;
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>(number)<per_n_lights>(number)";
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>(number)<per_n_lights>(number)" + lightType;
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>[<once><once_per_light>]";
    loadChildFromDefinition(iteration, passParamTree.get());
    iteration = "<iteration>[<once><once_per_light>]" + lightType;
    loadChildFromDefinition(iteration, passParamTree.get());

    // point_size
    std::string pointSize = "<point_size>(number)";
    loadChildFromDefinition(pointSize, passParamTree.get());

    // point_sprites
    std::string pointSprites = "<point_sprites>" + onOff;
    loadChildFromDefinition(pointSprites, passParamTree.get());

    // point_size_attenuation
    std::string pointSizeAttenuation = "<point_size_attenuation><off>";
    loadChildFromDefinition(pointSizeAttenuation, passParamTree.get());
    pointSizeAttenuation = "<point_size_attenuation><on>[<constant><linear><quadratic>]";
    loadChildFromDefinition(pointSizeAttenuation, passParamTree.get());

    // point_size_min
    std::string pointSizeMin = "<point_size_min>(number)";
    loadChildFromDefinition(pointSizeMin, passParamTree.get());

    // point_size_max
    std::string pointSizeMax = "<point_size_max>(number)";
    loadChildFromDefinition(pointSizeMax, passParamTree.get());

    // line_width
    std::string lineWidth = "<line_width>(number)";
    loadChildFromDefinition(lineWidth, passParamTree.get());
}

void OgreScriptLSP::ParamsValidator::setupTextureUnitParams() {
    textureUnitParamTree = std::make_unique<ParamsTree>();

    // texture_alias
    std::string textureAlias = "<texture_alias>(identifier)";
    loadChildFromDefinition(textureAlias, textureUnitParamTree.get());

    // texture
    std::string texture = "<texture>";
    loadChildFromDefinition(texture, textureUnitParamTree.get());
    texture = "<texture>(identifier)";
    loadChildFromDefinition(texture, textureUnitParamTree.get());
    texture = "<texture>(identifier)[<1d><2d><3d><cubic>]";
    loadChildFromDefinition(texture, textureUnitParamTree.get());
    texture = "<texture>(identifier)[<1d><2d><3d><cubic>][<unlimited>]";
    loadChildFromDefinition(texture, textureUnitParamTree.get());
    texture = "<texture>(identifier)[<1d><2d><3d><cubic>](number)";
    loadChildFromDefinition(texture, textureUnitParamTree.get());

    // anim_texture
    std::string animTexture = "<anim_texture>(identifier)(number)(number)";
    loadChildFromDefinition(animTexture, textureUnitParamTree.get());
    animTexture = "<anim_texture>";
    for (int i = 0; i < 20; ++i) {
        animTexture += "(identifier)";
        std::string tmp = animTexture + "(number)";
        loadChildFromDefinition(tmp, textureUnitParamTree.get());
    }

    // cubic_texture
    std::string cubicTexture = "<cubic_texture>(identifier)[<combinedUVW><separateUV>]";
    loadChildFromDefinition(cubicTexture, textureUnitParamTree.get());

    // content_type
    std::string contentType = "<content_type>[<named><shadow><compositor>]";
    loadChildFromDefinition(contentType, textureUnitParamTree.get());
    contentType = "<content_type>[<named><shadow><compositor>](identifier)";
    loadChildFromDefinition(contentType, textureUnitParamTree.get());
    contentType = "<content_type>[<named><shadow><compositor>](identifier)(identifier)";
    loadChildFromDefinition(contentType, textureUnitParamTree.get());
    contentType = "<content_type>[<named><shadow><compositor>](identifier)(identifier)(number)";
    loadChildFromDefinition(contentType, textureUnitParamTree.get());

    // tex_coord_set
    std::string texCoordSet = "<tex_coord_set>(number)";
    loadChildFromDefinition(texCoordSet, textureUnitParamTree.get());

    // colour_op
    std::string colourOp = "<colour_op>[<replace><add><modulate><alpha_blend>]";
    loadChildFromDefinition(colourOp, textureUnitParamTree.get());

    // colour_op_ex
    std::string colourOpExOp = "[<source1><source2><modulate><modulate_x2><modulate_x4><add><add_signed><add_smooth><subtract><blend_diffuse_alpha><blend_texture_alpha><blend_current_alpha><blend_manual><dotproduct><blend_diffuse_colour>]";
    std::string colourOpExSource = "[<current><texture><diffuse><specular><manual>]";
    std::string colourOpEx = "<colour_op_ex>" + colourOpExOp + colourOpExSource + colourOpExSource;
    loadChildFromDefinition(colourOpEx, textureUnitParamTree.get());

    // colour_op_multipass_fallback
    std::string sceneBlendFactor = "[<one><zero><dest_colour><source_colour><one_minus_dest_colour><one_minus_source_colour><dest_alpha><source_alpha><one_minus_dest_alpha><one_minus_source_alpha>]";
    std::string colourOpMultipassFallback = "<colour_op_multipass_fallback>" + sceneBlendFactor + sceneBlendFactor;
    loadChildFromDefinition(colourOpMultipassFallback, textureUnitParamTree.get());

    // alpha_op_ex
    std::string alphaOpEx = "<alpha_op_ex>" + colourOpExOp + colourOpExSource + colourOpExSource;
    loadChildFromDefinition(alphaOpEx, textureUnitParamTree.get());

    // env_map
    std::string envMap = "<env_map>[<off><spherical><planar><cubic_reflection><cubic_normal>]";
    loadChildFromDefinition(envMap, textureUnitParamTree.get());

    // scroll
    std::string scroll = "<scroll>(number)(number)";
    loadChildFromDefinition(scroll, textureUnitParamTree.get());

    // scroll_anim
    std::string scrollAnim = "<scroll_anim>(number)(number)";
    loadChildFromDefinition(scrollAnim, textureUnitParamTree.get());

    // rotate
    std::string rotate = "<rotate>(number)";
    loadChildFromDefinition(rotate, textureUnitParamTree.get());

    // rotate_anim
    std::string rotateAnim = "<rotate_anim>(number)";
    loadChildFromDefinition(rotateAnim, textureUnitParamTree.get());

    // scale
    std::string scale = "<scale>(number)(number)";
    loadChildFromDefinition(scale, textureUnitParamTree.get());

    // wave_xform
    std::string waveXform = "<wave_xform>[<scroll_x><scroll_y><rotate><scale_x><scale_y>][<sine><triangle><square><sawtooth><inverse_sawtooth><pwm>](number)(number)(number)(number)";
    loadChildFromDefinition(waveXform, textureUnitParamTree.get());

    // transform
    std::string transform = "<transform>(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)(number)";
    loadChildFromDefinition(transform, textureUnitParamTree.get());

    // unordered_access_mip
    std::string unorderedAccessMip = "<unordered_access_mip>(number)";
    loadChildFromDefinition(unorderedAccessMip, textureUnitParamTree.get());
}

void OgreScriptLSP::ParamsValidator::loadChildFromDefinition(std::string &definition, ParamsTree *treeRoot) {
    int pos = 0;
    ParamsTree *tree = treeRoot;

    std::vector<std::string> tkName = nextParamsToken(definition, pos);
    while (!tkName.empty()) {
        Token tk = identifier;
        if (std::any_of(tkName.begin(), tkName.end(), [](const std::string &n) { return n == "(number)"; })) {
            tk = number_literal;
        } else if (std::any_of(tkName.begin(), tkName.end(), [](const std::string &n) { return n == "(match)"; })) {
            tk = match_literal;
        }
        tree = tree->newChild(tkName, tk);
        tkName = nextParamsToken(definition, pos);
    }
    tree->markAsEnd();
}

std::vector<std::string> OgreScriptLSP::ParamsValidator::nextParamsToken(const std::string &definition, int &position) {
    consumeSpaces(definition, position);

    if (position >= definition.length()) {
        return {};
    }

    if (definition[position] == '<') {
        return {nextParamToken(definition, position)};
    } else if (definition[position] == '(') {
        std::string tokenName = "(" + nextParamToken(definition, position, ')') + ")";
        return {tokenName};
    }

    std::vector<std::string> tokens;
    position++;
    while (position < definition.length()) {
        consumeSpaces(definition, position);

        if (definition[position] == ']') {
            position++;
            break;
        } else if (definition[position] == '<') {
            std::string tk = nextParamToken(definition, position);
            if (!tk.empty()) {
                tokens.push_back(tk);
            }
        }
    }

    return tokens;
}

std::string OgreScriptLSP::ParamsValidator::nextParamToken( // NOLINT(*-convert-member-functions-to-static)
        const std::string &definition, int &position, char delimiter) {
    if (position >= definition.length()) {
        return "";
    }

    position++;
    std::string token;
    while (position < definition.length()) {
        if (definition[position] == delimiter) {
            position++;
            break;
        }
        token += definition[position++];
    }

    return token;
}

void OgreScriptLSP::ParamsValidator::consumeSpaces( // NOLINT(*-convert-member-functions-to-static)
        const std::string &definition, int &position) {
    while (position < definition.length() && definition[position] == ' ') position++;
}

OgreScriptLSP::ParamsTree *OgreScriptLSP::ParamsValidator::getPassParamTree() const {
    return passParamTree.get();
}
