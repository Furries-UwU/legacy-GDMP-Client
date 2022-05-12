#pragma once
#include <enet/enet.h>

class Global {
public:
    ENetPeer* peer;

    static auto& get() {
        static Global instance;
        return instance;
    }
};