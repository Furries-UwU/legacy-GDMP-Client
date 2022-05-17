#pragma once
#include "include.hpp"

#include <unordered_map>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>

struct SimplePlayerHolder
{
    SimplePlayer *playerOne;
    SimplePlayer *playerTwo;
};

struct PlayerData
{
    std::string username;
    IconData iconData;
    ColorData colorData;
};

class Global
{
protected:
    std::vector<std::function<void(void)>> gdThreadQueue;
    std::mutex gdThreadQueueMutex;

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