#include "multiplayerSimplePlayer.hpp"

MultiplayerSimplePlayer* MultiplayerSimplePlayer::create(int iconId) {
    auto simplePlayer = SimplePlayer::create(iconId);
    return dynamic_cast<MultiplayerSimplePlayer*>(simplePlayer);
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
                this->isPlayerOne ? playerData->second.renderData.playerOne
                                   : playerData->second.renderData.playerTwo;
        IconData iconData = playerData->second.iconData;

        this->setRotation(baseRenderData.rotation);
        this->setPosition({baseRenderData.position.x, baseRenderData.position.y});

#if defined(WIN32)
        ColorData colorData = playerData->second.colorData;
        this->setColor(
            ccc3(colorData.primaryColor.r,
                 colorData.primaryColor.g,
                 colorData.primaryColor.b));
        this->setSecondColor(
            ccc3(colorData.secondaryColor.r,
                 colorData.secondaryColor.g,
                 colorData.secondaryColor.b));
        this->setGlowOutline(colorData.glow);
#endif

        IconType iconType = Utility::getIconType(baseRenderData.gamemode);
        this->updatePlayerFrame(Utility::getIconId(iconType, iconData), iconType);
        this->setVisible((this->isPlayerOne && renderData.isVisible) || (renderData.isDual && renderData.isVisible));

//        (this->*m_usernameLabel)->setString(playerData->second.username.c_str());
}