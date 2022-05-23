#pragma once
#include "struct.hpp"
#include "enum.hpp"

#include <Geode.hpp>

class Utility
{
public:
    static Gamemode getGamemode(BaseRenderData);
    static Gamemode getGamemodeFromPlayer(PlayerObject*);
    static int getIconId(IconType, IconData);
    static IconType getIconType(BaseRenderData);
    static IconType getIconType(Gamemode);
};