#pragma once
#include "struct.hpp"
#include "enum.hpp"

#include <Geode.hpp>

class Utility
{
public:
    static Gamemode getGamemode(BaseRenderData renderData);
    static Gamemode getGamemodeFromPlayer(PlayerObject* player);
    static int Utility::getIconID(Gamemode gamemode);
    static IconType getIconType(BaseRenderData renderData);
    static IconType getIconType(Gamemode gamemode);
    /*static int getIconId(IconType iconType, IconData iconData);*/
};