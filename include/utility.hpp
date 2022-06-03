#pragma once

#include "include.hpp"
#include "global.hpp"

class Utility {
public:
    static Gamemode getGamemodeFromPlayer(PlayerObject *);

    static int getIconId(IconType, IconData);

    static IconType getIconType(Gamemode);

    static void sendUsername();
    static void sendIconData();
    static void sendColorData();
};