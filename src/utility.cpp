#include "utility.hpp"

Gamemode Utility::getGamemode(BaseRenderData renderData) {
    if (renderData.m_isShip) {
        return Gamemode::SHIP;
    } else if (renderData.m_isBird) {
        return Gamemode::UFO;
    } else if (renderData.m_isBall) {
        return Gamemode::BALL;
    } else if (renderData.m_isDart) {
        return Gamemode::WAVE;
    } else if (renderData.m_isRobot) {
        return Gamemode::ROBOT;
    } else if (renderData.m_isSpider) {
        return Gamemode::SPIDER;
    } else {
        return Gamemode::CUBE;
    }
}

IconType Utility::getIconType(BaseRenderData renderData)
{
    return Utility::getIconType(Utility::getGamemode(renderData));
}

IconType Utility::getIconType(Gamemode gamemode)
{
    switch (gamemode)
    {
    default:
    case NONE:
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

int Utility::getIconId(IconType iconType, ServerPlayerData playerData)
{
    switch (iconType)
    {
    default:
    case IconType::Cube:
        return playerData.cube;
    case IconType::Ship:
        return playerData.ship;
    case IconType::Ball:
        return playerData.ball;
    case IconType::Ufo:
        return playerData.bird;
    case IconType::Wave:
        return playerData.dart;
    case IconType::Robot:
        return  playerData.robot;
    case IconType::Spider:
        return playerData.spider;
    }
}