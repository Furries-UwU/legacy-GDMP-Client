#include "main.hpp"

USE_GEODE_NAMESPACE();

void onReceivedMessage(ENetPacket *enetPacket) {
    if (enetPacket->dataLength < 5) {
        fmt::print("Got invalid packet here");
        enet_packet_destroy(enetPacket);
        return;
    }

    Global *global = Global::get();

    Packet incomingPacket{enetPacket};

    switch (incomingPacket.type) {
        case (JOIN_LEVEL): {
            uint16_t playerId = *reinterpret_cast<uint16_t *>(incomingPacket.data);
            fmt::print("Join: {}\n", playerId);

            global->playerDataMap[playerId].playerId = playerId;

            executeInGDThread([playerId]() {
                Global *global = Global::get();

                auto playLayer = global->playLayer;
                if (!playLayer) {
                    fmt::print(stderr, "no PlayLayer? (cringe)\n");
                    return;
                }

                const auto objectLayer = playLayer->getObjectLayer();

#ifndef WIN32
                auto playerOne = MultiplayerPlayerObject::create(0, 0, playLayer);
                playerOne->playerId = playerId;
                playerOne->isPlayerOne = true;

                auto playerTwo = MultiplayerPlayerObject::create(0, 0, playLayer);
                playerTwo->playerId = playerId;
                playerTwo->isPlayerOne = false;
#else
                auto* playerOne = MultiplayerSimplePlayer::create(0);
                playerOne->playerId = playerId;
                playerOne->isPlayerOne = true;
                playerOne->scheduleUpdate();

                auto* playerTwo = MultiplayerSimplePlayer::create(0);
                playerTwo->playerId = playerId;
                playerTwo->isPlayerOne = false;
                playerTwo->scheduleUpdate();
#endif
                objectLayer->addChild(playerOne);
                objectLayer->addChild(playerTwo);
            });

            break;
        }

        /*
        case (ICON_DATA): {
            IncomingIconData incomingData = *reinterpret_cast<IncomingIconData *>(incomingPacket.data);
            global->playerDataMap[incomingData.playerId].iconData = incomingData.iconData;
            break;
        }
        case (COLOR_DATA): {
            IncomingColorData incomingData = *reinterpret_cast<IncomingColorData *>(incomingPacket.data);
            global->playerDataMap[incomingData.playerId].colorData = incomingData.colorData;
            break;
        }
         */
        case (RENDER_DATA): {
            IncomingRenderData incomingData = *reinterpret_cast<IncomingRenderData *>(incomingPacket.data);
            global->playerDataMap[incomingData.playerId].renderData = incomingData.renderData;

            fmt::print("r: {} {}\n", incomingData.renderData.playerOne.x, incomingData.renderData.playerOne.y);
            break;
        }

        case (LEAVE_LEVEL): {
            uint16_t playerId = *reinterpret_cast<uint16_t *>(incomingPacket.data);

            fmt::print("Leave: {}\n", playerId);

            global->playerDataMap.erase(playerId);
            break;
        }
    }

    enet_packet_destroy(enetPacket);
}

// PLEASE RUN THIS IN ANOTHER THREAD
[[noreturn]] void pollEvent() {
    while (true) {
        ENetEvent event;
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            Global *global = Global::get();

            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    onReceivedMessage(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    global->isConnected = true;
                    auto t0 = std::thread([]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        //Utility::sendColorData();
                        //Utility::sendIconData();
                    });
                    t0.detach();
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