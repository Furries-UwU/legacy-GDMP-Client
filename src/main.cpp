#include "main.hpp"

USE_GEODE_NAMESPACE();

void onRecievedMessage(ENetPacket *enetPacket) {
    if (enetPacket->dataLength < 5) {
        fmt::print("Got invalid packet here");
        enet_packet_destroy(enetPacket);
        return;
    }

    Global *global = Global::get();

    Packet packet;
    packet.ParseFromArray(enetPacket->data, enetPacket->dataLength);

    switch (packet.type()) {
        case (USERNAME): {
            IncomingUsername incomingUsername;
            incomingUsername.ParseFromString(packet.data());

            global->playerDataMap[incomingUsername.playerid()].username = incomingUsername.username();
            break;
        }
        case (ICON_DATA): {
            IncomingIconData incomingIconData;
            incomingIconData.ParseFromString(packet.data());

            global->playerDataMap[incomingIconData.playerid()].iconData = incomingIconData.icondata();
            break;
        }
        case (COLOR_DATA): {
            IncomingColorData incomingColorData;
            incomingColorData.ParseFromString(packet.data());

            global->playerDataMap[incomingColorData.playerid()].colorData = incomingColorData.colordata();
            break;
        }
        case (RENDER_DATA): {
            IncomingRenderData incomingRenderData;
            incomingRenderData.ParseFromString(packet.data());

            global->playerDataMap[incomingRenderData.playerid()].renderData = incomingRenderData.renderdata();
            break;
        }

        case (JOIN_LEVEL): {
            IncomingJoinLevel incomingJoinLevel;
            incomingJoinLevel.ParseFromString(packet.data());

            int playerId = incomingJoinLevel.playerid();

            fmt::print("Join: {}\n", playerId);

            executeInGDThread([playerId]() {
                Global *global = Global::get();

                auto playLayer = global->playLayer;
                if (!playLayer) {
                    fmt::print(stderr, "no PlayLayer? (cringe)\n");
                    return;
                }

                const auto objectLayer = playLayer->getObjectLayer();

                auto playerOne = MultiplayerSimplePlayer::create(0);
                playerOne->playerId = playerId;
                playerOne->isPlayerOne = true;


                auto playerTwo = MultiplayerSimplePlayer::create(0);
                playerTwo->playerId = playerId;
                playerTwo->isPlayerOne = false;

                objectLayer->addChild(playerOne);
                objectLayer->addChild(playerTwo);
            });

            break;
        }

        case (LEAVE_LEVEL): {
            IncomingLeaveLevel incomingLeaveLevel;
            incomingLeaveLevel.ParseFromString(packet.data());

            int playerId = incomingLeaveLevel.playerid();

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
                    onRecievedMessage(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    global->isConnected = true;
                    Utility::sendColorData();
                    Utility::sendIconData();
                    Utility::sendUsername();
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
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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