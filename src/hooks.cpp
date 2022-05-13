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

void(__thiscall* PlayLayer_onUpdate)(PlayLayer* self, float dt);
void __fastcall PlayLayer_onUpdate_H(PlayLayer* self, void*, float dt) {
    PlayLayer_onUpdate(self, dt);
	
    if (self->m_bIsPaused) return;

    PlayerObject* player1 = self->m_pPlayer1;
	PlayerObject* player2 = self->m_pPlayer2;

    RenderData renderData = {
        {
            player1->m_isShip,
            player1->m_isBird,
            player1->m_isBall,
			player1->m_isDart,
            player1->m_isRobot,
            player1->m_isSpider,
            player1->m_isUpsideDown,
            player1->m_isDashing,
            player1->m_position.x,
			player1->m_position.y,
			player1->getRotationX()
        },
        {
            player2->m_isShip,
            player2->m_isBird,
            player2->m_isBall,
            player2->m_isDart,
            player2->m_isRobot,
            player2->m_isSpider,
            player2->m_isUpsideDown,
            player2->m_isDashing,
            player2->m_position.x,
            player2->m_position.y,
            player2->getRotationX()
        },
        player1->isVisible() && player2->isVisible()
    };

    Packet(PLAYER_DATA, sizeof(renderData), (uint8_t*) &renderData).send(Global::get().peer);
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
    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x2029C0),
        reinterpret_cast<void*>(&PlayLayer_onUpdate_H),
        reinterpret_cast<void**>(&PlayLayer_onUpdate)
    );
}