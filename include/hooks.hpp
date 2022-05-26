#pragma once

#include <functional>
#include <vector>
#include <mutex>

#include "node/MultiplayerPlayerObject.hpp"
#include "layer/MultiplayerLayer.hpp"
#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"
#include "main.hpp"

USE_GEODE_NAMESPACE();

std::vector<std::function<void()>> functionQueue;
std::mutex threadMutex;

void executeInGDThread(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(threadMutex);
    functionQueue.push_back(std::move(f));
}

//class $modify(GJGarageLayer) {
//    void onBack(cocos2d::CCObject *pSender) {
//        GJGarageLayer::onBack(pSender);
//        Utility::sendColorData();
//        Utility::sendIconData();
//        Utility::sendUsername();
//    }
//};


class $modify(PlayerObject) {
//    field<CCLabelBMFont*> usernameLabel;

    bool init(int p0, int p1, cocos2d::CCLayer* p2) {
        if (!PlayerObject::init(p0, p1, p2)) return false;

        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto director = CCDirector::sharedDirector();
        auto size = director->getWinSize();

        auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        buttonSprite->setFlipX(true);

        auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MultiplayerLayer::switchToCustomLayerButton));

        auto menu = CCMenu::create();
        menu->addChild(button);
        menu->setPosition(size.width - 40, size.height - 40);

        this->addChild(menu);

        return true;
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);

        threadMutex.lock();
        auto buffer = std::move(functionQueue);
        threadMutex.unlock();

        for (auto &f: buffer)
            f();
    }
};

class $modify(PlayLayer) {

    bool init(GJGameLevel *level) {
        if (!PlayLayer::init(level)) return false;

        Global *global = Global::get();
        global->playLayer = this;

        if (global->isConnected) {
            JoinLevel joinLevel;
            joinLevel.set_levelid(level->m_levelID);

            Packet packet;
            packet.set_type(JOIN_LEVEL);
            packet.set_data(joinLevel.SerializeAsString());

            PacketUtility::sendPacket(packet, global->peer);
        }
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();

        Global *global = Global::get();
        global->playLayer = nullptr;
        global->playerDataMap.clear();

        if (global->isConnected) {
            Packet packet;
            packet.set_type(LEAVE_LEVEL);

            PacketUtility::sendPacket(packet, global->peer);
        }
    }

    void update(float p0) {
        PlayLayer::update(p0);

        Global *global = Global::get();

        if (!global->isConnected || this->m_isPaused || this->m_player1 == nullptr)
            return;

        PlayerObject *player1 = this->m_player1;
        PlayerObject *player2 = this->m_player2;

        Position playerOnePosition;
        playerOnePosition.set_x(player1->getPositionX());
        playerOnePosition.set_y(player1->getPositionY());


        BaseRenderData playerOneBaseRenderData;
        *playerOneBaseRenderData.mutable_position() = playerOnePosition;
        playerOneBaseRenderData.set_rotation(player1->getRotation());
        playerOneBaseRenderData.set_scale(player1->getScale());
        playerOneBaseRenderData.set_isvisible(player1->isVisible());
        playerOneBaseRenderData.set_gamemode(Utility::getGamemodeFromPlayer(player1));

        Position playerTwoPosition;
        playerTwoPosition.set_x(player2->getPositionX());
        playerTwoPosition.set_y(player2->getPositionY());

        BaseRenderData playerTwoBaseRenderData;
        *playerTwoBaseRenderData.mutable_position() = playerTwoPosition;
        playerTwoBaseRenderData.set_rotation(player2->getRotation());
        playerTwoBaseRenderData.set_scale(player2->getScale());
        playerTwoBaseRenderData.set_isvisible(player2->isVisible());
        playerTwoBaseRenderData.set_gamemode(Utility::getGamemodeFromPlayer(player2));

        RenderData renderData;
        *renderData.mutable_playerone() = playerOneBaseRenderData;
        *renderData.mutable_playertwo() = playerTwoBaseRenderData;

        Packet packet;
        packet.set_type(RENDER_DATA);
        packet.set_data(renderData.SerializeAsString());

        PacketUtility::sendPacket(packet, global->peer);
    }
};