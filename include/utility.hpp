#pragma once
#include "struct.hpp"
#include "enum.hpp"

#include <Geode.hpp>

class Utility
{
public:
    static Gamemode getGamemode(BaseRenderData);
    static Gamemode getGamemodeFromPlayer(PlayerObject*);
    static int getIconID(Gamemode);
    static IconType getIconType(BaseRenderData);
    static IconType getIconType(Gamemode);
    /*static int getIconId(IconType iconType, IconData iconData);*/
};