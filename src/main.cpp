#include "main.hpp"

USE_GEODE_NAMESPACE();

void sendIconData() {
    auto gm = GameManager::sharedState();

    IconData iconData = {
            gm->getPlayerFrame(),
            gm->getPlayerShip(),
            gm->getPlayerBall(),
            gm->getPlayerBird(),
            gm->getPlayerDart(),
            gm->getPlayerRobot(),
            gm->getPlayerSpider()
    };

    Packet(ICON_DATA, sizeof(iconData), reinterpret_cast<uint8_t *>(&iconData))
            .send(Global::get()->peer);
}

void sendColorData() {
    auto gm = GameManager::sharedState();

    auto primaryColor = gm->colorForIdx(gm->m_playerColor);
    auto secondaryColor = gm->colorForIdx(gm->m_playerColor2);

    ColorData colorData = {
            {primaryColor.r, primaryColor.g, primaryColor.b},
            {secondaryColor.r, secondaryColor.g, secondaryColor.b},
            gm->m_playerGlow
    };

    Packet(ICON_DATA, sizeof(colorData), reinterpret_cast<uint8_t *>(&colorData))
            .send(Global::get()->peer);
}

void updateIcon(SimplePlayer *simplePlayer, IconType iconType, IconData iconData) {
#if defined(WIN32) || !defined(MAC_EXPERIMENTAL)
    simplePlayer->updatePlayerFrame(Utility::getIconId(iconType, iconData), iconType);
#endif
}

void updateColor(SimplePlayer *simplePlayer, ColorData colorData) {
#if defined(WIN32) || !defined(MAC_EXPERIMENTAL)
    simplePlayer->setColor(
            ccc3(colorData.primaryColor.r,
                 colorData.primaryColor.g,
                 colorData.primaryColor.b));
    simplePlayer->setSecondColor(
            ccc3(colorData.secondaryColor.r,
                 colorData.secondaryColor.g,
                 colorData.secondaryColor.b));
    simplePlayer->setGlowOutline(colorData.glow);
#endif
}

#if defined(WIN32) || !defined(MAC_EXPERIMENTAL)

void updateRender(SimplePlayer *simplePlayer, BaseRenderData renderData) {
    simplePlayer->setPosition({renderData.position.x, renderData.position.y});
    simplePlayer->setRotation(renderData.rotation);
}

#else
void updateRender(PlayerObject *playerObject, BaseRenderData renderData) {
    playerObject->setPosition({renderData.position.x, renderData.position.y});
    playerObject->updateRotation(renderData.position.rotation);
}
#endif

void onRecievedMessage(ENetPacket *eNetPacket) {
    if (eNetPacket->dataLength < 5) {
        fmt::print("Got invalid packet here");
        enet_packet_destroy(eNetPacket);
        return;
    }

    Global *global = Global::get();
    auto packet = Packet(eNetPacket);

    fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", eNetPacket->dataLength,
               packet.type, packet.length);
    for (int x = 0; x < eNetPacket->dataLength; x++) {
        fmt::print(" {:#04x}", packet[x]);
    }
    fmt::print("\n\n");

    switch (packet.type) {
        case (ICON_DATA): {
            auto incomingIconData = *reinterpret_cast<IncomingIconData *>(packet.data);
            global->playerDataMap[incomingIconData.playerId].iconData = incomingIconData.iconData;
            break;
        }
        case (COLOR_DATA): {
            auto incomingColorData = *reinterpret_cast<IncomingColorData *>(packet.data);
            global->playerDataMap[incomingColorData.playerId].colorData = incomingColorData.colorData;

            auto playerHolder = global->playerHolderList[incomingColorData.playerId];

            if (playerHolder.playerOne)
                updateColor(playerHolder.playerOne, incomingColorData.colorData);
            if (playerHolder.playerTwo)
                updateColor(playerHolder.playerTwo, incomingColorData.colorData);

            break;
        }
        case (RENDER_DATA): {
            fmt::print("RENDER_DATA\n");

            auto incomingRenderData = *reinterpret_cast<IncomingRenderData *>(packet.data);
            fmt::print("Player {}: P1[{} {}]\t P2[{} {}]\n", incomingRenderData.playerId,
                       incomingRenderData.renderData.playerOne.position.x,
                       incomingRenderData.renderData.playerOne.position.y,
                       incomingRenderData.renderData.playerTwo.position.x,
                       incomingRenderData.renderData.playerTwo.position.y);

            auto check = global->playerHolderList.find(incomingRenderData.playerId);
            if (check == global->playerHolderList.end()) {
                fmt::print("no exist yes\n");
                break;
            }

            executeInGDThread([incomingRenderData]() {
                Global *global = Global::get();
                auto playerHolder = global->playerHolderList[incomingRenderData.playerId];

                fmt::print("update render 0 pid {}\n", incomingRenderData.playerId);

                if (playerHolder.playerOne) {
                    IconType iconType = Utility::getIconType(incomingRenderData.renderData.playerOne.gamemode);

                    updateRender(playerHolder.playerOne, incomingRenderData.renderData.playerOne);
                    playerHolder.playerOne->setVisible(incomingRenderData.renderData.isVisible);
                }

                if (playerHolder.playerTwo) {
                    IconType iconType = Utility::getIconType(incomingRenderData.renderData.playerTwo.gamemode);

                    updateRender(playerHolder.playerTwo, incomingRenderData.renderData.playerTwo);
                    playerHolder.playerTwo->setVisible(incomingRenderData.renderData.isDual);
                }
            });

            break;
        }

        case (JOIN_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Join: {}\n", playerId);

            executeInGDThread([playerId]() {
                Global *global = Global::get();

#if defined(WIN32) || !defined(MAC_EXPERIMENTAL)
                GameManager *gm = GameManager::sharedState();

                auto playLayer = global->playLayer;

                if (!playLayer) {
                    fmt::print(stderr, "no PlayLayer? (cringe)\n");
                    return;
                }

                const auto objectLayer = playLayer->getObjectLayer();

                auto *player1 = SimplePlayer::create(1);
                player1->updatePlayerFrame(0, IconType::Cube);
                player1->setVisible(true);


                auto *player2 = SimplePlayer::create(1);
                player2->updatePlayerFrame(0, IconType::Cube);
                player2->setVisible(false);

                objectLayer->addChild(player1);
                objectLayer->addChild(player2);
#else
                auto *player1 = PlayerObject::create(0, 0, nullptr);
                player1->addAllParticles();

                auto *player2 = PlayerObject::create(0, 0, nullptr);
                player2->addAllParticles();
#endif

                global->playerHolderList[playerId].playerOne = player1;
                global->playerHolderList[playerId].playerTwo = player2;
            });

            break;
        }

        case (LEAVE_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Leave: {}\n", playerId);

            executeInGDThread([playerId]() {
                Global *global = Global::get();

                if (global->playerHolderList[playerId].playerOne) {
                    global->playerHolderList[playerId].playerOne->setVisible(false);
                    global->playerHolderList[playerId].playerOne->removeMeAndCleanup();
                }

                if (global->playerHolderList[playerId].playerTwo) {
                    global->playerHolderList[playerId].playerTwo->setVisible(false);
                    global->playerHolderList[playerId].playerTwo->removeMeAndCleanup();
                }

                global->playerHolderList.erase(playerId);
                global->playerDataMap.erase(playerId);
            });

            break;
        }
    }

    enet_packet_destroy(eNetPacket);
}

// PLEASE RUN THIS IN ANOTHER THREAD
[[noreturn]] void pollEvent() {
    while (true) {
        ENetEvent event;
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            Global *global = Global::get();

            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    onRecievedMessage(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    global->isConnected = true;
                    fmt::print("Connected to server at port {}\n", Global::get()->host->address.port);
                    sendColorData();
                    sendIconData();
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    for (auto &player: global->playerHolderList) {
                        auto playerOne = player.second.playerOne;
                        auto playerTwo = player.second.playerTwo;

                        if (playerOne) {
                            playerOne->removeMeAndCleanup();
                        }

                        if (playerTwo) {
                            playerTwo->removeMeAndCleanup();
                        }

                        global->playerHolderList.erase(player.first);
                    }

                    global->playerDataMap.clear();
                    global->isConnected = false;

                    break;
                }
                case ENET_EVENT_TYPE_NONE: {
                }
            }
        }
    }
}

GEODE_API bool GEODE_CALL geode_load(Mod *mod) {
    if (enet_initialize() != 0) {
        fmt::print(stderr, "An error occurred while initializing ENet.\n");
        return false;
    }
    atexit(enet_deinitialize);

    Global *global = Global::get();
    global->host = enet_host_create(nullptr, 1, 1, 0, 0);

    std::thread eventThread(&pollEvent);
    eventThread.detach();

    return true;
}

GEODE_API void GEODE_CALL geode_unload() {
    enet_peer_disconnect(Global::get()->peer, 0);
    enet_host_destroy(Global::get()->host);
}