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
public:
    ENetHost *host;
    ENetPeer *peer;

    std::unordered_map<unsigned int, SimplePlayerHolder> simplePlayerObjectHolderList;
    std::unordered_map<unsigned int, ServerPlayerData> playerDataList;

    static Global *get();
};