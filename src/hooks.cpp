#include "hooks.hpp"

USE_GEODE_NAMESPACE();

class $modify(PlayLayer){

    bool init(GJGameLevel * level){
        if (!PlayLayer::init(level)) return false;
Packet(JOIN_LEVEL, 4, (uint8_t *)&level->m_levelID).send(Global::get()->interface, Global::get()->socket);
return true;
}

void onQuit()
{
  Global *global = Global::get();

  PlayLayer::onQuit();
  Packet(LEAVE_LEVEL).send(global->interface, global->socket);
}

void update(float p0)
{
  PlayLayer::update(p0);
  if (this->m_isPaused)
    return;
}
}
;

class $modify(CCScheduler){
    void update(float dt){
        CCScheduler::update(dt);
Global::get()->executeGDThreadQueue();
}
}
;