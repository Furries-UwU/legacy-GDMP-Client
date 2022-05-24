#pragma once

#include <functional>
#include <vector>
#include <mutex>

#include "layer/MultiplayerLayer.hpp"
#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"

USE_GEODE_NAMESPACE();

std::vector<std::function<void()>> functionQueue;
std::mutex threadMutex;

void executeInGDThread(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(threadMutex);
    functionQueue.push_back(std::move(f));
}

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

        if (global->isConnected)
            Packet(JOIN_LEVEL, sizeof(int), reinterpret_cast<uint8_t *>(&level->m_levelID)).send(global->peer);
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();

        Global *global = Global::get();
        global->playLayer = nullptr;
        global->playerDataMap.clear();

        if (global->isConnected)
            Packet(LEAVE_LEVEL).send(global->peer);
    }

    void update(float p0) {
        PlayLayer::update(p0);

        GameManager *gm = GameManager::sharedState();
        Global *global = Global::get();

        if (!global->isConnected || this->m_isPaused || this->m_player1 == nullptr)
            return;

        PlayerObject *player1 = this->m_player1;
        PlayerObject *player2 = this->m_player2;

        RenderData renderData{
                {
                        {player1->getPositionX(),
                         player1->getPositionY()},
                        player1->getRotation(),
                        Utility::getGamemodeFromPlayer(player1),
                },
                {
                        {player2->getPositionX(),
                         player2->getPositionY()},
                        player2->getRotation(),
                        Utility::getGamemodeFromPlayer(player2),
                },
                this->m_isDualMode,
                player1->isVisible() && player2->isVisible()};

        Packet(RENDER_DATA, sizeof(renderData), reinterpret_cast<uint8_t *>(&renderData)).send(global->peer);
    }
};