// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleObject.h"

#include "cSprite.h"
#include "cTransform.h"

#include <iostream>
#include <memory>

class oStaticSprite : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oStaticSprite)
public:
    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override {

        transform->ToJson(j_out["transform"]);
        sprite->ToJson(j_out["sprite"]);
    }

    void FromJson(const nlohmann::json& j_in) override {

        try {
            transform->FromJson(j_in.at("transform"));
            sprite->FromJson(j_in.at("sprite"));
        }
        catch (std::exception e) {
            std::cout << e.what();
        }
    }

    std::shared_ptr<cTransform> transform;
    std::shared_ptr<cSprite> sprite;

private:
    static inline int beginX = 0, beginY = 0;
};