#pragma once
#include "include.hpp"

#include <unordered_map>
#include <vector>

struct SimplePlayerHolder
{
    SimplePlayer *playerOne;
    SimplePlayer *playerTwo;
};

class Global
{
protected:
    std::vector<std::function<void()>> gdThreadQueue;

public:
    ENetHost *host;
    ENetPeer *peer;

    std::unordered_map<unsigned int, SimplePlayerHolder> simplePlayerObjectHolderList;
    std::unordered_map<unsigned int, ServerPlayerData> playerDataList;

    void queueInGDThread(std::function<void()> func);
    void executeGDThreadQueue();

    static Global *get();
};