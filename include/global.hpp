#pragma once

#include "include.hpp"

#include <unordered_map>

struct PlayerData {
    int playerId{};
    std::string username{};
    //IconData iconData{};
    //ColorData colorData{};
    RenderData renderData{};
};

class Global {
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    // playerId, PlayerData: list of all players connected
    std::unordered_map<uint16_t, PlayerData> playerDataMap;

    PlayLayer *playLayer = nullptr;

    bool isConnected = false;

    static Global *get();
};