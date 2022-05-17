#include "hooks.hpp"

USE_GEODE_NAMESPACE();

class $modify(PlayLayer) {

    bool init(GJGameLevel *level) {
        if (!PlayLayer::init(level)) return false;
        Packet(X2X_JOIN_LEVEL, 4, reinterpret_cast<uint8_t*>(&level->m_levelID)).send(Global::get()->peer); // manually specifying length is bad

        Global::get()->playLayer = this;

        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();

        Global *global = Global::get();

        global->playLayer = nullptr;

        for (auto &player: global->simplePlayerHolderList) {
            auto playerOne = player.second.playerOne;
            auto playerTwo = player.second.playerTwo;

            if (playerOne)
                playerOne->removeMeAndCleanup();

            if (playerTwo)
                playerTwo->removeMeAndCleanup();

            global->simplePlayerHolderList.erase(player.first);
        }

        global->playerDataMap.clear();

        Packet(X2X_LEAVE_LEVEL).send(global->peer);
    }

    void update(float p0) {
        PlayLayer::update(p0);
        if (this->m_isPaused)
            return;

        Global *global = Global::get();

        PlayerObject *player1 = this->m_player1;
        PlayerObject *player2 = this->m_player2;

        RenderData renderData{
                {
                player1 != NULL && player1->m_isShip,
                player1 != NULL && player1->m_isBird,
                player1 != NULL && player1->m_isBall,
                player1 != NULL && player1->m_isDart,
                player1 != NULL && player1->m_isRobot,
                player1 != NULL && player1->m_isSpider,
                player1 != NULL && player1->m_isUpsideDown,
                player1 != NULL && player1->m_isDashing,
                player1 ? player1->m_playerSpeed : 0.0f,
                player1->getPositionX(),
                player1->getPositionY(),
                player1->getRotation(),
                player1->getScale()
                },
                
                {
                player2 != NULL && player2->m_isShip,
                player2 != NULL && player2->m_isBird,
                player2 != NULL && player2->m_isBall,
                player2 != NULL && player2->m_isDart,
                player2 != NULL && player2->m_isRobot,
                player2 != NULL && player2->m_isSpider,
                player2 != NULL && player2->m_isUpsideDown,
                player2 != NULL && player2->m_isDashing,
                player2 ? player2->m_playerSpeed : 0.0f,
                player2 ? player2->getPositionX() : 0.0f,
                player2 ? player2->getPositionY() : 0.0f,
                player2 ? player2->getRotation() : 0.0f,
                player2 ? player2->getScale() : 0.0f
                },
                renderData.visible = this->m_player1->isVisible(),
                renderData.dual = player1->isVisible() && player2->isVisible() // this is probably a bad way to check if its dual
                };

        Packet(C2S_RENDER_DATA, sizeof(renderData), reinterpret_cast<uint8_t*>(&renderData)).send(global->peer);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);
        Global::get()->executeGDThreadQueue();
    }
};