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
