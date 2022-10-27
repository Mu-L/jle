// Copyright (c) 2022. Johan Lind

#include "cSpritesheetDepth.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jleQuadRendering.h"
#include "jleResource.h"
#include "json.hpp"

cSpritesheetDepth::cSpritesheetDepth(jleObject *owner, jleScene *scene)
    : cSpritesheet(owner, scene) {}

void cSpritesheetDepth::update(float dt) {
    if (!_spritesheet || !_quad.mtextureWithHeightmap) {
        return;
    }

    auto &texture = _spritesheet->_imageTexture;
    if (texture != nullptr && _hasEntity) {
        _quad.x = _transform->worldPosition().x +
                  _spritesheetEntityCache.sourceSize.x - _offset.x;
        _quad.y = _transform->worldPosition().y +
                  _spritesheetEntityCache.sourceSize.y - _offset.y;

        _quad.height = _spritesheetEntityCache.frame.height;
        _quad.width = _spritesheetEntityCache.frame.width;
        _quad.textureX = _spritesheetEntityCache.frame.x;
        _quad.textureY = _spritesheetEntityCache.frame.y;
        _quad.depth = _transform->worldPosition().z;

        if (_quad.mtextureWithHeightmap->normalmap) {
            gCore->quadRendering().sendTexturedHeightQuad(*&_quad);
        }
        else if (_quad.mtextureWithHeightmap->heightmap) {
            gCore->quadRendering().sendSimpleTexturedHeightQuad(*&_quad);
        }
    }
}

void cSpritesheetDepth::toJson(nlohmann::json &j_out) {
    cSpritesheet::toJson(j_out);
    j_out["_spritesheetPathDepth"] = _spritesheetPathDepth;
    j_out["_spritesheetPathDiffuse"] = _spritesheetPathDiffuse;
    j_out["_spritesheetPathNormal"] = _spritesheetPathNormal;
}

void cSpritesheetDepth::fromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _spritesheetPathNormal, "_spritesheetPathNormal", "");
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _spritesheetPathDiffuse, "_spritesheetPathDiffuse", "");
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _spritesheetPathDepth, "_spritesheetPathDepth", "");
    cSpritesheet::fromJson(j_in);

    if (_spritesheetPathDepth != "" && _spritesheetPathDiffuse != "") {
        createAndSetTextureFromPath(_spritesheetPathDiffuse,
                                    _spritesheetPathDepth,
                                    _spritesheetPathNormal);
    }
}
void cSpritesheetDepth::createAndSetTextureFromPath(
    const std::string &pathDiffuse,
    const std::string &pathHeight,
    const std::string &pathNormal) {
    if (!_quad.mtextureWithHeightmap) {
        _quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    _quad.mtextureWithHeightmap->texture =
        jleTexture::fromPath(jleRelativePath{pathDiffuse});
    _quad.mtextureWithHeightmap->heightmap =
        jleTexture::fromPath(jleRelativePath{pathHeight});
    if (!pathNormal.empty()) {
        _quad.mtextureWithHeightmap->normalmap =
            jleTexture::fromPath(jleRelativePath{pathNormal});
    }
}
