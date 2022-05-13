#pragma once
#include <unordered_map>
#include <enet/enet.h>

#include "struct.hpp"

#include <gd.h>
using namespace gd;

#include <cocos2d.h>
using namespace cocos2d;

class Global {
public:
    ENetPeer* peer;
	
    std::unordered_map<unsigned int, PlayerObject*> playerObjectList;
    std::unordered_map<unsigned int, ClientPlayerData> playerDataList;

    static auto& get() {
        static Global instance;
        return instance;
    }
};