#pragma once

#include "include.hpp"

#include <unordered_map>

struct PlayerData {
    std::string username = "Unknown";
    IconData iconData = {
            0, 0, 0, 0, 0, 0, 0
    };
    ColorData colorData = {
            255, 255, 255
    };
    RenderData renderData = {
            {
                    {
                            0, 0
                    },
                    0,
                    Gamemode::CUBE
            },
            {
                    {
                            0, 0
                    },
                    0,
                    Gamemode::CUBE
            },
            false,
            false
    };
};

class Global {
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    std::unordered_map<int, PlayerData> playerDataMap;

    PlayLayer *playLayer = nullptr;

    bool isConnected = false;

    static Global *get();
};