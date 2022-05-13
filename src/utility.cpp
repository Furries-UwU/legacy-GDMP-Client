#include "utility.hpp"

IconType Utility::getIconType(BaseRenderData renderData)
{
    if (renderData.m_isShip)
    {
        return IconType::Ship;
    }
    else if (renderData.m_isBird)
    {
        return IconType::Ship;
    }
    else if (renderData.m_isBall)
    {
        return IconType::Ball;
    }
    else if (renderData.m_isDart)
    {
        return IconType::Wave;
    }
    else if (renderData.m_isRobot)
    {
        return IconType::Robot;
    }
    else if (renderData.m_isSpider)
    {
        return IconType::Spider;
    }
    else
    {
        return IconType::Cube;
    }
}