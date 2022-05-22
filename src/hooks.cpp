#include "hooks.hpp"
#include <vector>
#include <mutex>
#include <functional>

USE_GEODE_NAMESPACE();

std::vector<std::function<void()>> functionQueue;
std::mutex threadMutex;

void executeInGDThread(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(threadMutex);
    functionQueue.push_back(std::move(f));
}

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);

        threadMutex.lock();
        auto buffer = std::move(functionQueue);
        threadMutex.unlock();

        for (auto& f : buffer) f();
    }
};

class $modify(PlayLayer) {

    bool init(GJGameLevel *level) {
        if (!PlayLayer::init(level)) return false;
        Packet(JOIN_LEVEL, sizeof(int), reinterpret_cast<uint8_t *>(&level->m_levelID)).send(
                Global::get()->peer); // manually specifying length is bad -rooot
                // Just use `sizeof(int)` -hayper

        Global::get()->playLayer = this;

        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();

        Global *global = Global::get();

        global->playLayer = nullptr;

        for (auto &player: global->playerHolderList) {
            auto playerOne = player.second.playerOne;
            auto playerTwo = player.second.playerTwo;

            if (playerOne)
                playerOne->removeMeAndCleanup();

            if (playerTwo)
                playerTwo->removeMeAndCleanup();

            global->playerHolderList.erase(player.first);
        }

        global->playerDataMap.clear();

        Packet(LEAVE_LEVEL).send(global->peer);
    }

    void update(float p0) {
        PlayLayer::update(p0);

        if (this->m_isPaused || this->m_player1 == nullptr)
            return;

        Global *global = Global::get();
        GameManager* gm = GameManager::sharedState();

        PlayerObject *player1 = this->m_player1;
        PlayerObject *player2 = this->m_player2;

        RenderData renderData{
            {
                {
                    player1->getPositionX(),
                    player1->getPositionY(),
                    player1->getRotation()
                },
                Utility::getIconID(Utility::getGamemodeFromPlayer(player1)),
                gm->getPlayerFrame(), // todo - this should do the thing do the yes thing ok
                Utility::getGamemodeFromPlayer(player1),
                player1->getScale(),
                gm->getPlayerGlow(), // todo - test if thats correct
                {
                    player1->getColor().r,
                    player1->getColor().g,
                    player1->getColor().b
                },
                {
                    /*player1->getSecondColor().r,
                    player1->getSecondColor().g,
                    player1->getSecondColor().b*/0, 0, 0
                }
            },
            {
                {
                    player2->getPositionX(),
                    player2->getPositionY(),
                    player2->getRotation()
                },
                Utility::getIconID(Utility::getGamemodeFromPlayer(player2)),
                gm->getPlayerFrame(),
                Utility::getGamemodeFromPlayer(player2),
                player2->getScale(),
                true, // todo - make this get the actual value if its glowing
                {
                    player2->getColor().r,
                    player2->getColor().g,
                    player2->getColor().b
                },
                {
                    /*player2->getSecondColor().r,
                    player2->getSecondColor().g,
                    player2->getSecondColor().b*/0, 0, 0
                }
            },
            player1->isVisible(),
            player1->isVisible() && player2->isVisible() // this is probably a bad way to check if its dual
        };

        Packet(RENDER_DATA, sizeof(renderData), reinterpret_cast<uint8_t *>(&renderData)).send(global->peer);
    }
};