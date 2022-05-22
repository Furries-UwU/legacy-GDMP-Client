#pragma once
#include "include.hpp"

#include <unordered_map>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>

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
    ENetHost *host;
    ENetPeer *peer;

    std::unordered_map<int, PlayerHolder> playerHolderList;
    std::unordered_map<int, PlayerData> playerDataMap;

    PlayLayer *playLayer = nullptr;

    static Global *get();
};