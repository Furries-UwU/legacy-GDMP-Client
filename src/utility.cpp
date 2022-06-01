#include "utility.hpp"

Gamemode Utility::getGamemodeFromPlayer(PlayerObject *player) {
#if WIN32
    if(player->m_isShip) {
        return Gamemode::SHIP;
    } else if(player->m_isBall) {
        return Gamemode::BALL;
    } else if(player->m_isBird) {
        return Gamemode::UFO;
    } else if(player->m_isDart) {
        return Gamemode::WAVE;
    } else if(player->m_isRobot) {
        return Gamemode::ROBOT;
    } else if(player->m_isSpider) {
        return Gamemode::SPIDER;
    } else {
        return Gamemode::CUBE;
    }
#else
    return Gamemode::CUBE;
#endif
}

IconType Utility::getIconType(Gamemode gamemode) {
    switch (gamemode) {
        default:
        case CUBE:
            return IconType::Cube;
        case SHIP:
            return IconType::Ship;
        case BALL:
            return IconType::Ball;
        case UFO:
            return IconType::Ufo;
        case WAVE:
            return IconType::Wave;
        case ROBOT:
            return IconType::Robot;
        case SPIDER:
            return IconType::Spider;
    }
}


int Utility::getIconId(IconType iconType, IconData iconData) {
    switch (iconType) {
        default:
        case IconType::Cube:
            return iconData.cubeid();
        case IconType::Ship:
            return iconData.shipid();
        case IconType::Ball:
            return iconData.ballid();
        case IconType::Ufo:
            return iconData.ufoid();
        case IconType::Wave:
            return iconData.waveid();
        case IconType::Robot:
            return iconData.robotid();
        case IconType::Spider:
            return iconData.spiderid();
    }
}

void Utility::sendUsername() {
    /*
    auto global = Global::get();
    auto gm = GameManager::sharedState();

    Packet packet;
    packet.set_bytedata(std::string(gm->m_playerName));

    packet.set_type(USERNAME);

    PacketUtility::sendPacket(global->peer, packet);
    */
}

void Utility::sendIconData() {
    auto global = Global::get();
    auto gm = GameManager::sharedState();

    IconData iconData;
    iconData.set_cubeid(gm->getPlayerFrame());
    iconData.set_shipid(gm->getPlayerShip());
    iconData.set_ballid(gm->getPlayerBall());
    iconData.set_ufoid(gm->getPlayerBird());
    iconData.set_waveid(gm->getPlayerDart());
    iconData.set_ballid(gm->getPlayerBall());
    iconData.set_robotid(gm->getPlayerRobot());
    iconData.set_spiderid(gm->getPlayerSpider());

    Packet packet;
    packet.set_type(ICON_DATA);
    packet.mutable_icondata()->CopyFrom(iconData);

    PacketUtility::sendPacket(global->peer, packet);
}

void Utility::sendColorData() {
    auto gm = GameManager::sharedState();

    auto primaryColorCocos = gm->colorForIdx(gm->getPlayerColor());
    auto secondaryColorCocos = gm->colorForIdx(gm->getPlayerColor2());

    Color primaryColor;
    primaryColor.set_r(primaryColorCocos.r);
    primaryColor.set_g(primaryColorCocos.g);
    primaryColor.set_b(primaryColorCocos.b);

    Color secondaryColor;
    secondaryColor.set_r(secondaryColorCocos.r);
    secondaryColor.set_g(secondaryColorCocos.g);
    secondaryColor.set_b(secondaryColorCocos.b);

    ColorData colorData;
    colorData.mutable_primarycolor()->CopyFrom(primaryColor);
    colorData.mutable_secondarycolor()->CopyFrom(secondaryColor);
    colorData.set_glow(gm->getPlayerGlow());

    Packet packet;
    packet.set_type(COLOR_DATA);
    packet.mutable_colordata()->CopyFrom(colorData);

    PacketUtility::sendPacket(Global::get()->peer, packet);
}