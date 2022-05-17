#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAddress, int port) {
    Global *global = Global::get();

    ENetAddress address;
    ENetPeer *peer;

    enet_address_set_host(&address, ipAddress);
    address.port = port;

    global->peer = enet_host_connect(global->host, &address, 1, 0);
    if (global->peer == NULL) {
        fmt::print(stderr,
                   "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
}

void updateRender(SimplePlayer *simplePlayer, BaseRenderData renderData) {
    fmt::print("updateRender {}\n", simplePlayer == nullptr);
    simplePlayer->setPosition({renderData.posX, renderData.posY});
    fmt::print("updateRender 1\n");
    simplePlayer->setRotation(renderData.rotation);
    fmt::print("updateRender 2\n");
    simplePlayer->setScale(renderData.scale);
}

void OnRecievedMessage(ENetPacket *eNetPacket) {
    if (eNetPacket->dataLength < 5) {
        enet_packet_destroy(eNetPacket);
        return;
    }

    auto packet = Packet(eNetPacket);

    fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", eNetPacket->dataLength,
               packet.type, packet.length);
    for (int x = 0; x < eNetPacket->dataLength; x++) {
        fmt::print(" {:#04x}", packet[x]);
    }
    fmt::print("\n\n");

    switch (packet.type) {
        case (S2C_UPDATE_PLAYER_RENDER_DATA): {
            fmt::print("S2C_UPDATE_PLAYER_RENDER_DATA\n");
            auto incomingRenderData = *reinterpret_cast<PlayerRenderData*>(packet.data);
            fmt::print("Player {}: P1[{} {}]\t P2[{} {}]\n", incomingRenderData.playerId,
                                       incomingRenderData.playerOne.posX, incomingRenderData.playerOne.posY,
                                       incomingRenderData.playerTwo.posX, incomingRenderData.playerTwo.posY);

            Global::get()->queueInGDThread([incomingRenderData]() {
                Global *global = Global::get();
                SimplePlayerHolder playerHolder = global->simplePlayerHolderList[incomingRenderData.playerId];
                
                fmt::print("update render 0 pid {}\n", incomingRenderData.playerId);
                if (playerHolder.playerOne) {
                    updateRender(playerHolder.playerOne, incomingRenderData.playerOne);
                    playerHolder.playerOne->setVisible(incomingRenderData.visible);
                }
                
                if (playerHolder.playerTwo) {
                    updateRender(playerHolder.playerTwo, incomingRenderData.playerTwo);
                    playerHolder.playerTwo->setVisible(incomingRenderData.dual);
                }
            });
            
            break;
        }

        case (X2X_JOIN_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("join: {}\n", playerId);

            Global::get()->queueInGDThread([playerId]() {
                Global *global = Global::get();

                auto playLayer = global->playLayer;

                if (!playLayer) {
                    fmt::print("no playlayer? (cringe)\n");
                    return;
                }

                if(global->simplePlayerHolderList[playerId].playerOne != nullptr) {
                    global->simplePlayerHolderList.erase(playerId);
                }

                const auto objectLayer = playLayer->getObjectLayer();

                SimplePlayer *player1 = SimplePlayer::create(1);
                player1->updatePlayerFrame(1, IconType::Cube);
                player1->setVisible(true);

                SimplePlayer *player2 = SimplePlayer::create(1);
                player2->updatePlayerFrame(1, IconType::Cube);
                player2->setVisible(false);

                objectLayer->addChild(player1);
                objectLayer->addChild(player2);

                global->simplePlayerHolderList[playerId].playerOne = player1;
                global->simplePlayerHolderList[playerId].playerTwo = player2;
            });

            break;
        }
    }

    enet_packet_destroy(eNetPacket);
}

// PLEASE RUN THIS IN ANOTHER THREAD
void pollEvent() {
    while (true) {
        ENetEvent event;
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    OnRecievedMessage(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    fmt::print("Connected to server at port {}\n", Global::get()->host->address.port);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    fmt::print("Disconnected from server!\n");
                    break;
                }
                case ENET_EVENT_TYPE_NONE: { // idk what this is supposed to be
                    break;
                }
            }
        }
    }
}

GEODE_API bool GEODE_CALL geode_load(Mod *mod) {
    if (enet_initialize() != 0) {
        fmt::print(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    Global *global = Global::get();
    global->host = enet_host_create(nullptr, 1, 1, 0, 0);

    connect("127.0.0.1", 23973);

    std::thread eventThread(&pollEvent);
    eventThread.detach();

    return true;
}

GEODE_API void GEODE_CALL geode_unload() {
    enet_host_destroy(Global::get()->host);
}