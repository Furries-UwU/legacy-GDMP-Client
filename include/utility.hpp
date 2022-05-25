#pragma once

#include "include.hpp"

class Utility {
public:
    static Gamemode getGamemodeFromPlayer(PlayerObject *);

    static int getIconId(IconType, IconData);

    static IconType getIconType(Gamemode);
};