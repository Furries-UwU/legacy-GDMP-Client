#include "hooks.hpp"

bool(__thiscall* PlayLayer_init)(PlayLayer* self, GJGameLevel* level);
bool __fastcall PlayLayer_init_H(PlayLayer* self, void*, GJGameLevel* level) {
	Packet(JOIN_LEVEL, 4, (uint8_t*)&level->m_nLevelID).send(Global::get().peer);
    return PlayLayer_init(self, level);
}

void(__thiscall* PlayLayer_onQuit)(PlayLayer* self);
void __fastcall PlayLayer_onQuit_H(PlayLayer* self) {
    PlayLayer_onQuit(self);
    Packet(LEAVE_LEVEL).send(Global::get().peer);
}

void createHook() {
    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x01FB780),
        reinterpret_cast<void*>(&PlayLayer_init_H),
        reinterpret_cast<void**>(&PlayLayer_init)
    );	
    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x20D810),
        reinterpret_cast<void*>(&PlayLayer_onQuit_H),
        reinterpret_cast<void**>(&PlayLayer_onQuit)
    );
}