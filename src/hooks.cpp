#include "hooks.hpp"

USE_GEODE_NAMESPACE();

void SendPlayerData() {
  auto gameManager = GameManager::sharedState();

  ServerPlayerData serverPlayerData = {
    gameManager -> getPlayerFrame(),
    gameManager -> getPlayerShip(),
    gameManager -> getPlayerBall(),
    gameManager -> getPlayerBird(),
    gameManager -> getPlayerDart(),
    gameManager -> getPlayerRobot(),
    gameManager -> getPlayerSpider(),
    gameManager -> getPlayerGlow(),
    gameManager -> getPlayerColor(),
    gameManager -> getPlayerColor2()
  };

  Packet(PLAYER_DATA, sizeof(serverPlayerData), reinterpret_cast < uint8_t * > ( & serverPlayerData))
    .send(Global::get() -> peer);
}

void updateRender(SimplePlayer * simplePlayer, ServerPlayerData playerData, BaseRenderData renderData) {
  IconType iconType = Utility::getIconType(renderData);

  simplePlayer -> updatePlayerFrame(Utility::getIconId(iconType, playerData), iconType);
  simplePlayer -> updatePlayerFrame(1, iconType);
  simplePlayer -> setPosition({
    renderData.posX,
    renderData.posY
  });
  simplePlayer -> setRotation(renderData.rotation);
  simplePlayer -> setScale(renderData.scale);
}

void updateSkin(SimplePlayer * simplePlayer, ServerPlayerData playerData, bool swapColor = false) {
  GameManager * gm = GameManager::sharedState();

  _ccColor3B primaryColor = gm -> colorForIdx(playerData.primaryColor);
  _ccColor3B secondaryColor = gm -> colorForIdx(playerData.secondaryColor);

  simplePlayer -> setColor(swapColor ? secondaryColor : primaryColor);
  simplePlayer -> setSecondColor(swapColor ? primaryColor : secondaryColor);
  simplePlayer -> updateColors();
  simplePlayer -> setGlowOutline(playerData.glow);
}

void OnRecievedPacket(ENetEvent event) {
  Packet recievedPacket = Packet(event.packet);

  fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", event.packet -> dataLength, recievedPacket.type, recievedPacket.length);
  for (int x = 0; x < event.packet -> dataLength; x++) {
    fmt::print(" {:#04x}", recievedPacket[x]);
  }
  fmt::print("\n\n");

  switch (recievedPacket.type) {
  case PLAYER_LEAVE_LEVEL: {
    unsigned int playerId = *
      reinterpret_cast < unsigned int * > (recievedPacket.data);

    Global * global = Global::get();
    SimplePlayerHolder holder = global -> simplePlayerObjectHolderList[playerId];

    if (holder.playerOne)
      holder.playerOne -> removeMeAndCleanup();
    if (holder.playerTwo)
      holder.playerTwo -> removeMeAndCleanup();

    global -> playerDataList.erase(playerId);
    global -> simplePlayerObjectHolderList.erase(playerId);

    break;
  }
  case PLAYER_JOIN_LEVEL: {
    Global * global = Global::get();
    auto playLayer = GameManager::sharedState() -> getPlayLayer();

    if (!playLayer)
      break;

    auto objectLayer = playLayer -> getObjectLayer();

    if (!objectLayer)
      break;

    uint32_t playerId = * reinterpret_cast < uint32_t * > (recievedPacket.data);

    ServerPlayerData serverPlayerData = global -> playerDataList[playerId];

    SimplePlayer * player1 = SimplePlayer::create(global -> playerDataList[playerId].cube);
    player1 -> updatePlayerFrame(1, IconType::Cube);
    player1 -> updatePlayerFrame(Utility::getIconId(IconType::Cube, serverPlayerData), IconType::Cube);

    SimplePlayer * player2 = SimplePlayer::create(global -> playerDataList[playerId].cube);
    player2 -> updatePlayerFrame(1, IconType::Cube);
    player2 -> updatePlayerFrame(Utility::getIconId(IconType::Cube, serverPlayerData), IconType::Cube);
    player2 -> setVisible(false);

    updateSkin(player1, serverPlayerData);
    updateSkin(player2, serverPlayerData, true);

    objectLayer -> addChild(player1);
    objectLayer -> addChild(player2);

    global -> simplePlayerObjectHolderList[playerId].playerOne = player1;
    global -> simplePlayerObjectHolderList[playerId].playerTwo = player2;

    break;
  }
  case UPDATE_PLAYER_RENDER_DATA: {
    PlayerRenderData renderData = * reinterpret_cast < PlayerRenderData * > (recievedPacket.data);

    GameManager * gm = GameManager::sharedState();

    Global * global = Global::get();
    SimplePlayerHolder holder = global -> simplePlayerObjectHolderList[renderData.playerId];

    SimplePlayer * player1 = holder.playerOne;
    SimplePlayer * player2 = holder.playerTwo;

    ServerPlayerData serverPlayerData = global -> playerDataList[renderData.playerId];

    if (player1) {
      updateSkin(player1, serverPlayerData);
      updateRender(player1, serverPlayerData, renderData.playerOne);

      player1 -> setVisible(renderData.visible);
    }

    if (player2) {
      updateSkin(player2, serverPlayerData, true);
      updateRender(player2, serverPlayerData, renderData.playerTwo);

      player2 -> setVisible(renderData.dual);
    }

    break;
  }
  case UPDATE_PLAYER_DATA: {
    ClientPlayerData clientPlayerData = *
      reinterpret_cast < ClientPlayerData * > (recievedPacket.data);
    Global::get() -> playerDataList[clientPlayerData.playerId] = {
      // TODO: Find a better way
      clientPlayerData.cube,
      clientPlayerData.ship,
      clientPlayerData.ball,
      clientPlayerData.bird,
      clientPlayerData.dart,
      clientPlayerData.robot,
      clientPlayerData.spider,
      clientPlayerData.glow,
      clientPlayerData.primaryColor,
      clientPlayerData.secondaryColor
    };
    break;
  }
  }
}

class $modify(PlayLayer) {

  bool init(GJGameLevel * level) {
    if (!PlayLayer::init(level)) return false;
    Packet(JOIN_LEVEL, 4, (uint8_t * ) & level -> m_levelID).send(Global::get() -> peer);

    return true;
  }

  void onQuit() {
    Global * global = Global::get();

    PlayLayer::onQuit();
    Packet(LEAVE_LEVEL).send(global -> peer);

    for (auto it = global -> simplePlayerObjectHolderList.begin(); it != global -> simplePlayerObjectHolderList.end(); it++) {
      SimplePlayerHolder holder = it -> second;
      if (holder.playerOne)
        holder.playerOne -> removeMeAndCleanup();
      if (holder.playerTwo)
        holder.playerTwo -> removeMeAndCleanup();
    }

    global -> playerDataList.clear();
    global -> simplePlayerObjectHolderList.clear();
  }

  void update(float p0) {
    PlayLayer::update(p0);
    if (this -> m_isPaused)
      return;

    PlayerObject * player1 = this -> m_player1;
    PlayerObject * player2 = this -> m_player2;

    RenderData renderData = {
      {
        player1 -> m_isShip,
          player1 -> m_isBird,
          player1 -> m_isBall,
          player1 -> m_isDart,
          player1 -> m_isRobot,
          player1 -> m_isSpider,
          player1 -> m_isUpsideDown,
          player1 -> m_isDashing,
          player1 -> m_playerSpeed,
          player1 -> getPositionX(),
          player1 -> getPositionY(),
          player1 -> getRotation(),
          player1 -> getScale()
      },
      {
        player2 ? player2 -> m_isShip : false,
        player2 ? player2 -> m_isBird : false,
        player2 ? player2 -> m_isBall : false,
        player2 ? player2 -> m_isDart : false,
        player2 ? player2 -> m_isRobot : false,
        player2 ? player2 -> m_isSpider : false,
        player2 ? player2 -> m_isUpsideDown : false,
        player2 ? player2 -> m_isDashing : false,
        player2 ? player2 -> m_playerSpeed : 0.0f,
        player2 ? player2 -> getPositionX() : 0.0f,
        player2 ? player2 -> getPositionY() : 0.0f,
        player2 ? player2 -> getRotationX() : 0.0f,
        player2 ? player2 -> getScale() : 0.0f
      },
      player1 -> isVisible() && player2 -> isVisible(),
      player1 && player2
    };

    Packet(RENDER_DATA, sizeof(renderData), (uint8_t * ) & renderData)
      .send(Global::get() -> peer);
  }
};

class $modify(CCScheduler) {
  void update(float dt) {
    CCScheduler::update(dt);

    ENetHost * host = Global::get() -> host;
    if (!host) return;

    ENetEvent event;

    while (enet_host_service(host, & event, 0) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_DISCONNECT:
        for (auto v: Global::get() -> simplePlayerObjectHolderList) {
          v.second.playerOne -> removeMeAndCleanup();
          v.second.playerTwo -> removeMeAndCleanup();
        };

        Global::get() -> playerDataList.clear();
        Global::get() -> simplePlayerObjectHolderList.clear();
        break;
      case ENET_EVENT_TYPE_CONNECT:
        fmt::print("Connected!\n");
        SendPlayerData();
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        OnRecievedPacket(event);
        enet_packet_destroy(event.packet);
        break;
      }
    }
  }
};