#include "node/MultiplayerSimplePlayer.hpp"

bool MultiplayerSimplePlayer::init(int iconID) {
    if (!SimplePlayer::init(iconID)) return false;

//    this->usernameLabel = CCLabelBMFont::create("Unknown", "bigFont.fnt");
//    this->usernameLabel->setScale(0.25);
//    this->addChild(this->getParent());
    return true;
}

MultiplayerSimplePlayer* MultiplayerSimplePlayer::create(int iconID) {
    auto* pRet = new MultiplayerSimplePlayer();
    if (pRet && pRet->init(iconID)) {
        //pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        return nullptr;
    }
}

void MultiplayerSimplePlayer::update(float dt) {
    SimplePlayer::update(dt);

    Global *global = Global::get();

    auto playerData = global->playerDataMap.find(this->playerId);

    if (playerData == global->playerDataMap.end() || !global->isConnected) {
        this->removeMeAndCleanup();
        return;
    }

    BaseRenderData baseRenderData = this->isPlayerOne ? playerData->second.renderData.playerOne : playerData->second.renderData.playerTwo;

    this->setRotation(baseRenderData.rotation);
    this->setPosition({baseRenderData.x, baseRenderData.y});

    IconType iconType = Utility::getIconType(baseRenderData.gamemode);
    this->updatePlayerFrame(Utility::getIconId(iconType, playerData->second.iconData), iconType);

    /*
    auto primaryColor = playerData->second.colorData.primaryColor;
    auto secondaryColor = playerData->second.colorData.secondaryColor;
    this->setColor(ccc3(primaryColor.r, primaryColor.g, primaryColor.b));
    this->setSecondColor(ccc3(secondaryColor.r, secondaryColor.g, secondaryColor.b));
     */

    this->setVisible(baseRenderData.visible);
}