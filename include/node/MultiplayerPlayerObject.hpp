#pragma once

#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"

USE_GEODE_NAMESPACE();

class MultiplayerPlayerObject : public PlayerObject {
public:
    int playerId = 0;
    bool isPlayerOne = false;

    static MultiplayerPlayerObject *create(int, int, CCLayer*);

    bool init(int p0, int p1, CCLayer* p2);

    void update(float delta) override;
};