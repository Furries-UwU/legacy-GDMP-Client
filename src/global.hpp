#pragma once
#include <unordered_map>
#include "include.hpp"

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

    static auto &get()
    {
        static Global instance;
        return instance;
    }
};