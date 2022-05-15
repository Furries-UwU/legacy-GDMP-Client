#include "utility.hpp"

Gamemode Utility::getGamemode(BaseRenderData renderData) {
    if (renderData.isShip) {
        return Gamemode::SHIP;
    } else if (renderData.isUFO) {
        return Gamemode::UFO;
    } else if (renderData.isBall) {
        return Gamemode::BALL;
    } else if (renderData.isWave) {
        return Gamemode::WAVE;
    } else if (renderData.isRobot) {
        return Gamemode::ROBOT;
    } else if (renderData.isSpider) {
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

int Utility::getIconId(IconType iconType, IconData iconData)
{
    switch (iconType)
    {
    default:
    case IconType::Cube:
        return iconData.cubeId;
    case IconType::Ship:
        return iconData.shipId;
    case IconType::Ball:
        return iconData.ballId;
    case IconType::Ufo:
        return iconData.ufoId;
    case IconType::Wave:
        return iconData.waveId;
    case IconType::Robot:
        return  iconData.robotId;
    case IconType::Spider:
        return iconData.spiderId;
    }
}