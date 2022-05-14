#pragma once
#include "include.hpp"

#include <unordered_map>

struct SimplePlayerHolder
{
    SimplePlayer *playerOne;
    SimplePlayer *playerTwo;
};

class Global
{
public:
    ENetHost *host;
    ENetPeer *peer;

    std::unordered_map<unsigned int, SimplePlayerHolder> simplePlayerObjectHolderList;
    std::unordered_map<unsigned int, ServerPlayerData> playerDataList;

    PlayLayer *playLayer;

    static auto &get()
    {
        static Global instance;
        return instance;
    }
};