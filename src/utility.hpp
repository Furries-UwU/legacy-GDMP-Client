#pragma once
#include "struct.hpp"
#include "enum.hpp"

#include <Geode.hpp>

class Utility
{
public:
    static Gamemode getGamemode(BaseRenderData renderData);
    static IconType getIconType(BaseRenderData renderData);
    static IconType getIconType(Gamemode gamemode);
    static int getIconId(IconType iconType, IconData iconData);
};