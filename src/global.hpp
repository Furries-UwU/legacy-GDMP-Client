#pragma once
#include <unordered_map>
#include "include.hpp"

struct PlayerObjectHolder {
    PlayerObject* playerOne;
    PlayerObject* playerTwo;
};

class Global {
public:
    ENetHost* host;
    ENetPeer* peer;
	
    std::unordered_map<unsigned int, PlayerObjectHolder> playerObjectHolderList;
    std::unordered_map<unsigned int, ServerPlayerData> playerDataList;

    static auto& get() {
        static Global instance;
        return instance;
    }
};