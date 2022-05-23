#pragma once
#include "include.hpp"

#include <unordered_map>

#if defined(WIN32) || !defined(MAC_EXPERIMENTAL)
struct PlayerHolder
{
    SimplePlayer *playerOne;
    SimplePlayer *playerTwo;
};
#else
struct PlayerHolder
{
    PlayerObject *playerOne;
    PlayerObject *playerTwo;
};
#endif

struct PlayerData
{
    std::string username;
};

class Global
{
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    std::unordered_map<int, PlayerHolder> playerHolderList;
    std::unordered_map<int, PlayerData> playerDataMap;

    PlayLayer *playLayer = nullptr;

    static Global *get();
};