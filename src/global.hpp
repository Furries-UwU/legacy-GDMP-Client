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
    IconData iconData;
    ColorData colorData;
};

class Global
{
protected:
    std::vector<std::function<void(void)>> gdThreadQueue;

public:
    ISteamNetworkingSockets *interface;
    HSteamListenSocket connection;
    HSteamNetPollGroup pollGroup;

    std::unordered_map<int, SimplePlayerHolder> simplePlayerHolderList;
    std::unordered_map<int, PlayerData> playerDataMap;

    void queueInGDThread(std::function<void()> func);
    void executeGDThreadQueue();

    static Global *get();
};