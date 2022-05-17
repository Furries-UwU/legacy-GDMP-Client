#pragma once
#include "include.hpp"

#include <unordered_map>
#include <functional>
#include <thread>
#include <vector>

struct SimplePlayerHolder
{
    SimplePlayer *playerOne;
    SimplePlayer *playerTwo;
};

struct PlayerData
{
    std::string username;
    PlayerRenderData renderData;
};

class Global
{
protected:
    std::vector<std::function<void(void)>> gdThreadQueue;

public:
    ENetHost *host;
    ENetPeer *peer;

    std::unordered_map<int, SimplePlayerHolder> simplePlayerHolderList;
    std::unordered_map<int, PlayerData> playerDataMap;

    PlayLayer *playLayer;

    void queueInGDThread(const std::function<void()>& func);
    void executeGDThreadQueue();

    static Global *get();
};