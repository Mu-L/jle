// Copyright (c) 2022. Johan Lind

#ifndef JLE_SCENE_H
#define JLE_SCENE_H

#include <memory>
#include <vector>

#include "jleJson.h"
#include "jlePath.h"
#include <json.hpp>

class jleObject;

class jleScene : public jleJsonInterface<nlohmann::json> {
public:
    jleScene();

    explicit jleScene(const std::string& sceneName);

    virtual ~jleScene() = default;

    template <typename T>
    std::shared_ptr<T> SpawnObject();

    std::shared_ptr<jleObject> SpawnObject(const std::string& objName);

    std::shared_ptr<jleObject> SpawnObject(const nlohmann::json& j_in);

    std::shared_ptr<jleObject> SpawnTemplateObject(
        const jleRelativePath& templatePath);

    void UpdateSceneObjects(float dt);

    void ProcessNewSceneObjects();

    virtual void SceneUpdate() {}

    virtual void OnSceneCreation() {}

    virtual void OnSceneDestruction() {}

    void DestroyScene();

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    bool bPendingSceneDestruction = false;

    std::vector<std::shared_ptr<jleObject>>& GetSceneObjects();

    std::string mSceneName;

protected:
    friend class jleObject;

    std::vector<std::shared_ptr<jleObject>> mSceneObjects;
    std::vector<std::shared_ptr<jleObject>> mNewSceneObjects;

    friend void to_json(nlohmann::json& j, jleScene& s);

    friend void from_json(const nlohmann::json& j, jleScene& s);

private:
    static int mScenesCreatedCount;

    void ConfigurateSpawnedObject(const std::shared_ptr<jleObject>& obj);
};

void to_json(nlohmann::json& j, jleScene& s);

void from_json(const nlohmann::json& j, jleScene& s);

#include "jleScene.inl"

#endif // JLE_SCENE_H