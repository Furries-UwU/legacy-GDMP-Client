#pragma once

#include "include.hpp"

#include <unordered_map>

struct PlayerData {
    int playerId;
    std::string username;
    IconData iconData;
    ColorData colorData;
    RenderData renderData;
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