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


#ifndef WIN32
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
#endif

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

        fmt::print("init level\n");
        Global *global = Global::get();
        global->playLayer = this;

        if (global->isConnected) {
            fmt::print("level id {}\n", level->m_levelID);

            int32_t levelId = level->m_levelID;
            Packet packet{JOIN_LEVEL, sizeof(levelId), reinterpret_cast<uint8_t *>(&levelId)};
            packet.send(global->peer);
            Utility::sendColorData();
            Utility::sendIconData();
            fmt::print("sent join packet\n");
        } else {
            fmt::print("not connected!\n");
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
            packet.type = LEAVE_LEVEL;
            packet.length = 0;
            fmt::print("send packet! a(leave)\n");
            packet.send(global->peer);
        }
    }

    void update(float p0) {
        PlayLayer::update(p0);

        Global *global = Global::get();

        if (!global->isConnected || this->m_isPaused || this->m_player1 == nullptr)
            return;

        PlayerObject *player1 = this->m_player1;
        PlayerObject *player2 = this->m_player2;

        BaseRenderData p1Render{player1->getPositionX(),
                                player1->getPositionY(),
                                player1->getRotation(),
                                player1->getScale(),
                                player1->isVisible(),
                                Utility::getGamemodeFromPlayer(player1)};

        BaseRenderData p2Render{player2->getPositionX(),
                                player2->getPositionY(),
                                player2->getRotation(),
                                player2->getScale(),
                                player2->isVisible(),
                                Utility::getGamemodeFromPlayer(player2)};

        RenderData renderData{p1Render, p2Render};

        Packet packet {RENDER_DATA, sizeof(renderData), reinterpret_cast<uint8_t *>(&renderData)};
        packet.send(global->peer);
    }
};