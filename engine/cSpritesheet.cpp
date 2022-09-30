// Copyright (c) 2022. Johan Lind

#include "cSpritesheet.h"
#include "jleCore.h"
#include "jleGameEngine.h"
#include "jlePathDefines.h"
#include "jleQuadRenderingInterface.h"
#include "jleResourceHolder.h"

#include <plog/Log.h>

void cSpritesheet::Start() {
    mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);
}

cSpritesheet::cSpritesheet(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

void cSpritesheet::Update(float dt) {
    if (!mSpritesheet) {
        return;
    }

    auto& texture = mSpritesheet->mImageTexture;
    if (texture != nullptr && mHasEntity) {
        TexturedQuad quad{texture};
        quad.x = mTransform->GetWorldX();
        quad.y = mTransform->GetWorldY();

        quad.height = mSpritesheetEntityCache.mFrame.mHeight;
        quad.width = mSpritesheetEntityCache.mFrame.mWidth;
        quad.textureX = mSpritesheetEntityCache.mFrame.mX;
        quad.textureY = mSpritesheetEntityCache.mFrame.mY;
        quad.depth = mTransform->GetWorldDepth();

        if (quad.texture.get()) {
            jleCore::core->rendering->quads->SendTexturedQuad(
                quad, RenderingMethod::Dynamic);
        }
    }
}

void cSpritesheet::ToJson(nlohmann::json& j_out) {
    j_out["mSpritesheetPath"] = mSpritesheetPath;
    j_out["mSpriteName"] = mSpriteName;
}

void cSpritesheet::FromJson(const nlohmann::json& j_in) {
    mSpritesheetPath = j_in["mSpritesheetPath"];
    mSpriteName = j_in["mSpriteName"];
    mSpritesheet = jleResourceHolder::LoadResourceFromFile<jleSpritesheet>(
        jleRelativePath{mSpritesheetPath});

    const auto entity = mSpritesheet->mSpritesheetEntities.find(mSpriteName);
    if (entity != mSpritesheet->mSpritesheetEntities.end()) {
        mSpritesheetEntityCache = entity->second;
        mHasEntity = true;
    }
    else {
        LOGW << "Could not find sprite entity on the spritesheet: "
             << mSpriteName;
    }
}