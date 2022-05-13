#include "hooks.hpp"

USE_GEODE_NAMESPACE();

SimplePlayer *simplePlayer;

class $modify(PlayLayer){

    bool init(GJGameLevel * level){
        if (!PlayLayer::init(level)) return false;
Packet(JOIN_LEVEL, 4, (uint8_t *)&level->m_levelID).send(Global::get().peer);

simplePlayer = SimplePlayer::create(1);
this->m_objectLayer->addChild(simplePlayer);

return true;
}

void onQuit()
{
    PlayLayer::onQuit();
    Packet(LEAVE_LEVEL).send(Global::get().peer);

    Global global = Global::get();

    for (auto it = global.playerObjectHolderList.begin(); it != global.playerObjectHolderList.end(); it++)
    {
        SimplePlayerHolder holder = it->second;
        if (holder.playerOne)
            holder.playerOne->removeFromParent();
        if (holder.playerTwo)
            holder.playerTwo->removeFromParent();
    }

    simplePlayer->updatePlayerFrame(1, IconType::Cube);
    global.playerDataList.clear();
    global.playerObjectHolderList.clear();
}

void update(float p0)
{
    PlayLayer::update(p0);
    if (this->m_isPaused)
        return;

    PlayerObject *player1 = this->m_player1;
    PlayerObject *player2 = this->m_player2;

    RenderData renderData = {
        {player1->m_isShip,
         player1->m_isBird,
         player1->m_isBall,
         player1->m_isDart,
         player1->m_isRobot,
         player1->m_isSpider,
         player1->m_isUpsideDown,
         player1->m_isDashing,
         player1->m_playerSpeed,
         player1->getPositionX(),
         player1->getPositionY(),
         player1->getRotation(),
         player1->getScale()},
        {player2 ? player2->m_isShip : false,
         player2 ? player2->m_isBird : false,
         player2 ? player2->m_isBall : false,
         player2 ? player2->m_isDart : false,
         player2 ? player2->m_isRobot : false,
         player2 ? player2->m_isSpider : false,
         player2 ? player2->m_isUpsideDown : false,
         player2 ? player2->m_isDashing : false,
         player2 ? player2->m_playerSpeed : 0.0f,
         player2 ? player2->getPositionX() : 0.0f,
         player2 ? player2->getPositionY() : 0.0f,
         player2 ? player2->getRotationX() : 0.0f,
         player2 ? player2->getScale() : 0.0f},
        player1->isVisible() && player2->isVisible(),
        player1 && player2};

    Packet(RENDER_DATA, sizeof(renderData), (uint8_t *)&renderData)
        .send(Global::get().peer);
}
}
;