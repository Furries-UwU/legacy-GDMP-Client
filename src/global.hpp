#pragma once
#include <enet/enet.h>

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