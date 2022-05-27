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
            IncomingPacket incomingUsernamePacket;
            incomingUsernamePacket.ParseFromString(packet.data());

            global->playerDataMap[incomingUsernamePacket.playerid()].username = incomingUsernamePacket.data();
            break;
        }
        case (ICON_DATA): {
            IncomingPacket incomingIconDataPacket;
            incomingIconDataPacket.ParseFromString(packet.data());
            
            IconData iconData;
            iconData.ParseFromString(packet.data());

            global->playerDataMap[incomingIconDataPacket.playerid()].iconData = iconData;
            break;
        }
        case (COLOR_DATA): {
            IncomingPacket incomingColorDataPacket;
            incomingColorDataPacket.ParseFromString(packet.data());

            ColorData colorData;
            colorData.ParseFromString(incomingColorDataPacket.data());

            global->playerDataMap[incomingColorDataPacket.playerid()].colorData = colorData;
            break;
        }
        case (RENDER_DATA): {
            IncomingPacket incomingRenderDataPacket;
            incomingRenderDataPacket.ParseFromString(packet.data());

            RenderData renderData;
            renderData.ParseFromString(incomingRenderDataPacket.data());

            global->playerDataMap[incomingRenderDataPacket.playerid()].renderData = renderData;
            break;
        }

        case (JOIN_LEVEL): {
            IncomingPacket incomingJoinLevelPacket;
            incomingJoinLevelPacket.ParseFromString(packet.data());

            int playerId = incomingJoinLevelPacket.playerid();

            fmt::print("Join: {}\n", playerId);

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
                auto playerOne = MultiplayerSimplePlayer::create(0);
                playerOne->playerId = playerId;
                playerOne->isPlayerOne = true;

                auto playerTwo = MultiplayerSimplePlayer::create(0);
                playerTwo->playerId = playerId;
                playerTwo->isPlayerOne = false;
#endif
                objectLayer->addChild(playerOne);
                objectLayer->addChild(playerTwo);
            });

            break;
        }

        case (LEAVE_LEVEL): {
            IncomingPacket incomingLeaveLevelPacket;
            incomingLeaveLevelPacket.ParseFromString(packet.data());

            int playerId = incomingLeaveLevelPacket.playerid();

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