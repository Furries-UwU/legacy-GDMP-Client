#include "node/MultiplayerSimplePlayer.hpp"

MultiplayerSimplePlayer *MultiplayerSimplePlayer::create(int iconID) {
    auto pRet = new MultiplayerSimplePlayer();
    if (pRet && pRet->init(iconID)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

void MultiplayerSimplePlayer::update(float dt) {
    SimplePlayer::update(dt);

    fmt::print("This is a test");

    Global *global = Global::get();

    auto playerData = global->playerDataMap.find(this->playerId);

    if (playerData == global->playerDataMap.end() || !global->isConnected) {
        this->removeMeAndCleanup();
        return;
    }

    RenderData renderData = playerData->second.renderData;
    BaseRenderData baseRenderData =
            this->isPlayerOne ? playerData->second.renderData.playerone()
                              : playerData->second.renderData.playertwo();
    IconData iconData = playerData->second.iconData;

    this->setRotation(baseRenderData.rotation());
    this->setPosition({baseRenderData.position().x(), baseRenderData.position().y()});

    IconType iconType = Utility::getIconType(baseRenderData.gamemode());
    this->updatePlayerFrame(Utility::getIconId(iconType, iconData), iconType);
    this->setVisible(baseRenderData.isvisible());

//        (this->*m_usernameLabel)->setString(playerData->second.username.c_str());
}