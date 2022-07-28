// Copyright (c) 2022. Johan Lind

#include "hexNetScene.h"
#include "oCharacter.h"

void hexNetScene::OnNetConnected() {
    std::cout << "Net connected\n";
}

void hexNetScene::OnNetFailed() {
    std::cout << "Net failure\n";
}

void hexNetScene::OnNetClosed(const std::string &reason) {
    std::cout << "Net closed: " << reason << '\n';
}

void hexNetScene::SceneUpdate() {
    jleNetScene::SceneUpdate();
}

void
hexNetScene::ProcessNetMessage(const std::string &event, const nlohmann::json &message, const std::string &sender) {
    jleNetScene::ProcessNetMessage(event, message, sender);

    if(event == "player_connect")
    {
        const std::string id = message["id"];
        GetPlayerFromId(id); // will also create the player
    }

    if(event == "oFireball")
    {
        const int id = message["id"];
        auto fb = GetFireballFromId(id).lock();
        fb->FromNet(message);
    }

    if (event == "player_pos") {
        if (mPlayers.find(sender) == mPlayers.end()) {
            std::weak_ptr<jle::jleObject> newPlayer = SpawnTemplateObject("GR:/otemps/otherPlayer.tmpl");
            mPlayers[sender] = newPlayer;
        }

        if (auto otherPlayer = mPlayers[sender].lock()) {
            int q = message["q"];
            int r = message["r"];
            int d = message["d"];
            std::static_pointer_cast<oCharacter>(otherPlayer)->SetHexagonPlacementInterp(q, r);
            std::static_pointer_cast<oCharacter>(otherPlayer)->SetCharacterDirection(
                    static_cast<oCharacter::oCharacterDirection>(d));
        } else {
            mPlayers.erase(sender);
        }

        return;
    }

    if (event == "basic_attack") {
        auto player = GetPlayerFromId(sender).lock();
        int q = message["q"];
        int r = message["r"];
        int d = message["d"];
        std::static_pointer_cast<oCharacter>(player)->SetHexagonPlacementTeleport(q, r);
        std::static_pointer_cast<oCharacter>(player)->Attack(static_cast<oCharacter::oCharacterDirection>(d));
    }

    if(event == "player_dconnect")
    {
        const std::string id = message["id"];
        auto player = GetPlayerFromId(id).lock();
        player->DestroyObject();
    }

}

std::weak_ptr<jle::jleObject> hexNetScene::GetPlayerFromId(const std::string &id) {
    auto it = mPlayers.find(id);
    if (it == mPlayers.end() || (it != mPlayers.end() && it->second.expired())) {
        std::weak_ptr<jle::jleObject> newPlayer = SpawnTemplateObject("GR:/otemps/otherPlayer.tmpl");
        mPlayers[id] = newPlayer;
        return newPlayer;
    }
    return it->second.lock();
}

std::weak_ptr<oFireball> hexNetScene::GetFireballFromId(const int id) {
    auto it = mFireballs.find(id);
    if (it == mFireballs.end() || (it != mFireballs.end() && it->second.expired())) {
        auto newFireball = std::static_pointer_cast<oFireball>(SpawnTemplateObject("GR:otemps/FireballTempl.tmpl"));
        mFireballs[id] = newFireball;
        return newFireball;
    }
    return it->second.lock();
}