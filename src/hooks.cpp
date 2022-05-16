#include "hooks.hpp"

USE_GEODE_NAMESPACE();

class $modify(PlayLayer) {

    bool init(GJGameLevel *level) {
        if (!PlayLayer::init(level)) return false;
        Packet(JOIN_LEVEL, 4, (uint8_t *) &level->m_levelID).sendPacket(Global::get()->peer);
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();

        Global *global = Global::get();

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

        Packet(LEAVE_LEVEL).sendPacket(global->peer);
    }

    void update(float p0) {
        PlayLayer::update(p0);
        if (this->m_isPaused)
            return;

        Global *global = Global::get();

        RenderData renderData;

        renderData.playerOne = {
                {this->m_player1->getPositionX(),
                 this->m_player1->getPositionY()},
                this->m_player1->getRotation(),
                this->m_player1->getScale(),
                this->m_player1->m_isShip,
                this->m_player1->m_isBall,
                this->m_player1->m_isBird,
                this->m_player1->m_isDart,
                this->m_player1->m_isRobot,
                this->m_player1->m_isSpider};

        renderData.playerTwo = {
                {this->m_player2 ? this->m_player2->getPositionX() : 0.0f,
                 this->m_player2 ? this->m_player1->getPositionY() : 0.0f},
                this->m_player2 ? this->m_player1->getRotation() : 0.0f,
                this->m_player2 ? this->m_player1->getScale() : 0.0f,
                this->m_player2 != NULL && this->m_player2->m_isShip,
                this->m_player2 != NULL && this->m_player2->m_isBall,
                this->m_player2 != NULL && this->m_player2->m_isBird,
                this->m_player2 != NULL && this->m_player2->m_isDart,
                this->m_player2 != NULL && this->m_player2->m_isRobot,
                this->m_player2 != NULL && this->m_player2->m_isSpider};

        renderData.isDual = this->m_player1->isVisible() && this->m_player2->isVisible();
        renderData.isVisible = this->m_player1->isVisible();

        auto renderDataBson = json::to_bson(json(renderData));

        Packet(RENDER_DATA, renderDataBson.size(),
               reinterpret_cast<uint8_t *>(renderDataBson.data()))
                .sendPacket(global->peer);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);
        Global::get()->executeGDThreadQueue();
    }
};