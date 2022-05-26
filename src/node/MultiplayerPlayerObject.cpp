#include "node/MultiplayerPlayerObject.hpp"

bool MultiplayerPlayerObject::init(int p0, int p1, CCLayer* p2) {
    if (!PlayerObject::init(p0, p1, p2)) return false;

//    this->usernameLabel = CCLabelBMFont::create("Unknown", "bigFont.fnt");
//    this->usernameLabel->setScale(0.25);
//    this->usernameLabel->setPosition(ccp(this->getPositionX(), this->getPositionY()+5));
    return true;
}

MultiplayerPlayerObject *MultiplayerPlayerObject::create(int cubeId, int shipId, CCLayer* layer) {
    auto pRet = new MultiplayerPlayerObject();
    if (pRet && pRet->init(cubeId, shipId, layer)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

void MultiplayerPlayerObject::update(float dt) {
    PlayerObject::update(dt);

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
    ColorData colorData = playerData->second.colorData;

    this->setRotation(baseRenderData.rotation());
    this->setPosition({baseRenderData.position().x(), baseRenderData.position().y()});

    this->updatePlayerFrame(iconData.cubeid());
    this->updatePlayerBirdFrame(iconData.ufoid());
    this->updatePlayerDartFrame(iconData.waveid());
    this->updatePlayerRobotFrame(iconData.robotid());
    this->updatePlayerShipFrame(iconData.shipid());
    this->updatePlayerSpiderFrame(iconData.spiderid());
    this->updatePlayerRollFrame(iconData.ballid());

    this->toggleBirdMode(baseRenderData.gamemode() == UFO);
    this->toggleDartMode(baseRenderData.gamemode() == WAVE);
    this->toggleRobotMode(baseRenderData.gamemode() == ROBOT);
    this->toggleFlyMode(baseRenderData.gamemode() == SHIP);
    this->toggleSpiderMode(baseRenderData.gamemode() == SPIDER);
    this->toggleRollMode(baseRenderData.gamemode() == BALL);

    this->setGlowOpacity(colorData.glow() ? 255 : 0);

    this->setVisible(baseRenderData.isvisible());
    this->setScale(baseRenderData.scale());

//    this->usernameLabel->setString(playerData->second.username.c_str());
//    this->usernameLabel->setPosition(ccp(this->getPositionX(), this->getPositionY()+5));
}