// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleScene.h"

typedef void librg_world;
typedef void librg_event;

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

class jleSceneNetworked : public jleScene
{
protected:
    static void trackEntityObject(librg_world *world, int64_t entityId, const std::shared_ptr<jleObject> &object);
    void trackEntityObject(int64_t entityId, const std::shared_ptr<jleObject> &object);

    static void untrackEntityObject(librg_world *world, int64_t entityId);
    void untrackEntityObject(int64_t entityId);

    static void resetNetEntityToObjectPointer(librg_world *world, int64_t entityId);

    static void setNetEntityToObjectPointer(librg_world *world, int64_t entityId, std::weak_ptr<jleObject> object);
    void setNetEntityToObjectPointer(int64_t entityId, std::weak_ptr<jleObject> object);

    static std::shared_ptr<jleObject> getObjectPointerFromNetEntity(librg_world *world, int64_t entityId);
    std::shared_ptr<jleObject> getObjectPointerFromNetEntity(int64_t entityId);

    virtual void processNetwork() = 0;

    static void networkSceneDisplayInspectorWindow(const std::string &sceneType,
                                                   const std::string &sceneName,
                                                   ENetHost *host,
                                                   librg_world *world);

    librg_world *_world = nullptr;
};
