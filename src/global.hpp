#pragma once
#include <unordered_map>
#include <enet/enet.h>

#include "struct.hpp"

#include <gd.h>
using namespace gd;

#include <cocos2d.h>
using namespace cocos2d;

struct PlayerObjectHolder {
    PlayerObject* playerOne;
    PlayerObject* playerTwo;
};

class Global {
public:
    ENetPeer* peer;
	
    std::unordered_map<unsigned int, PlayerObjectHolder> playerObjectHolderList;
    std::unordered_map<unsigned int, ServerPlayerData> playerDataList;

    static auto& get() {
        static Global instance;
        return instance;
    }
};