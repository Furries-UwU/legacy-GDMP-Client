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

    auto primaryColor = gm->colorForIdx(gm->getPlayerColor());
    auto secondaryColor = gm->colorForIdx(gm->getPlayerColor2());

    ColorData colorData = {
            {primaryColor.r, primaryColor.g, primaryColor.b},
            {secondaryColor.r, secondaryColor.g, secondaryColor.b},
            gm->m_playerGlow
    };

    Packet(COLOR_DATA, sizeof(colorData), reinterpret_cast<uint8_t *>(&colorData))
            .send(Global::get()->peer);
}

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
            break;
        }
        case (RENDER_DATA): {
            auto incomingRenderData = *reinterpret_cast<IncomingRenderData *>(packet.data);
            global->playerDataMap[incomingRenderData.playerId].renderData = incomingRenderData.renderData;
            break;
        }

        case (JOIN_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Join: {}\n", playerId);

            executeInGDThread([playerId]() {
                Global *global = Global::get();

                auto playLayer = global->playLayer;
                if (!playLayer) {
                    fmt::print(stderr, "no PlayLayer? (cringe)\n");
                    return;
                }

                const auto objectLayer = playLayer->getObjectLayer();

                auto player1 = dynamic_cast<MultiplayerSimplePlayer*>(SimplePlayer::create(0));
                player1->playerId() = playerId;
                player1->isPlayerOne() = true;
                player1->isMultiplayer() = true;


                auto player2 = dynamic_cast<MultiplayerSimplePlayer*>(SimplePlayer::create(0));
                player2->playerId() = playerId;
                player2->isPlayerOne() = false;
                player2->isMultiplayer() = true;

                objectLayer->addChild(player1);
                objectLayer->addChild(player2);
            });

            break;
        }

        case (LEAVE_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Leave: {}\n", playerId);
            global->playerDataMap.erase(playerId);
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
                    sendColorData();
                    sendIconData();
                    fmt::print("Connected to server at port {}\n", Global::get()->host->address.port);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
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