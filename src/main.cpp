#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAddress, int port) {
    Global *global = Global::get();

    ENetAddress address;
    enet_address_set_host(&address, ipAddress);
    address.port = port;

    global->peer = enet_host_connect(global->host, &address, 1, 0);
    if (global->peer == nullptr) {
        fmt::print(stderr,
                   "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
}

void updateRender(SimplePlayer *simplePlayer, BaseRenderData renderData) {
    simplePlayer->setPosition({renderData.position.x, renderData.position.y});
    simplePlayer->setRotation(renderData.position.rotation);
    simplePlayer->setScale(renderData.iconData.scale);
    simplePlayer->updatePlayerFrame(renderData.iconData.iconId, Utility::getIconType(renderData));
    #ifdef _WIN32
    simplePlayer->setColor(
        ccc3(renderData.iconData.primaryColor.red,
            renderData.iconData.primaryColor.green,
            renderData.iconData.primaryColor.blue));
    #endif
    simplePlayer->setSecondColor(
        ccc3(renderData.iconData.secondaryColor.red,
            renderData.iconData.secondaryColor.green,
            renderData.iconData.secondaryColor.blue));
}

void onRecievedMessage(ENetPacket *eNetPacket) {
    if (eNetPacket->dataLength < 5) {
        fmt::print("Got invalid packet here");
        enet_packet_destroy(eNetPacket);
        return;
    }

    auto packet = Packet(eNetPacket);

    fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", eNetPacket->dataLength, packet.type, packet.length);
    for (int x = 0; x < eNetPacket->dataLength; x++) {
        fmt::print(" {:#04x}", packet[x]);
    }
    fmt::print("\n\n");

    switch (packet.type) {
        case (RENDER_DATA): {
            fmt::print("RENDER_DATA\n");
            
            auto incomingRenderData = *reinterpret_cast<IncomingRenderData *>(packet.data);
            fmt::print("Player {}: P1[{} {}]\t P2[{} {}]\n", incomingRenderData.playerId,
                       incomingRenderData.renderData.playerOne.position.x,
                       incomingRenderData.renderData.playerOne.position.y,
                       incomingRenderData.renderData.playerTwo.position.x,
                       incomingRenderData.renderData.playerTwo.position.y);

            Global *global = Global::get();

            auto check = global->simplePlayerHolderList.find(incomingRenderData.playerId);
            if(check == global->simplePlayerHolderList.end()) {
                fmt::print("no exist yes\n");
                break;
            }

            addCallback([incomingRenderData]() {
                Global *global = Global::get();
                SimplePlayerHolder playerHolder = global->simplePlayerHolderList[incomingRenderData.playerId];

                fmt::print("update render 0 pid {}\n", incomingRenderData.playerId);
                if (playerHolder.playerOne) {
                    updateRender(playerHolder.playerOne, incomingRenderData.renderData.playerOne);
                    playerHolder.playerOne->setVisible(incomingRenderData.renderData.isVisible);
                }

                if (playerHolder.playerTwo) {
                    updateRender(playerHolder.playerTwo, incomingRenderData.renderData.playerTwo);
                    playerHolder.playerTwo->setVisible(incomingRenderData.renderData.isDual);
                }
            });

            break;
        }

        case (JOIN_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Join: {}\n", playerId);

            addCallback([playerId]() {
                Global *global = Global::get();

                auto playLayer = global->playLayer;

                if (!playLayer) {
                    fmt::print(stderr, "no PlayLayer? (cringe)\n");
                    return;
                }

                if (global->simplePlayerHolderList[playerId].playerOne != nullptr) {
                    global->simplePlayerHolderList.erase(playerId);
                } // todo: check if this if statement works

                const auto objectLayer = playLayer->getObjectLayer();

                SimplePlayer *player1 = SimplePlayer::create(1);
                player1->updatePlayerFrame(1, IconType::Cube);
                player1->setVisible(true);

                SimplePlayer *player2 = SimplePlayer::create(1);
                player2->updatePlayerFrame(1, IconType::Cube);
                player2->setVisible(false);

                objectLayer->addChild(player1);
                objectLayer->addChild(player2);

                if(player1 != nullptr)
                    global->simplePlayerHolderList[playerId].playerOne = player1;
                
                if(player2 != nullptr)
                    global->simplePlayerHolderList[playerId].playerTwo = player2;
            });

            break;
        }

        case (LEAVE_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);
            fmt::print("Leave: {}\n", playerId);

            addCallback([playerId]() {
                Global *global = Global::get();

                if(global->simplePlayerHolderList[playerId].playerOne != nullptr) {
                    global->simplePlayerHolderList[playerId].playerOne->setVisible(false);
                    global->simplePlayerHolderList[playerId].playerOne->removeMeAndCleanup();
                }

                if(global->simplePlayerHolderList[playerId].playerTwo != nullptr) {
                    global->simplePlayerHolderList[playerId].playerTwo->setVisible(false);
                    global->simplePlayerHolderList[playerId].playerTwo->removeMeAndCleanup();
                }

                global->simplePlayerHolderList.erase(playerId);
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
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    onRecievedMessage(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    fmt::print("Connected to server at port {}\n", Global::get()->host->address.port);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    Global *global = Global::get();

                    for (auto &player: global->simplePlayerHolderList) {
                        auto playerOne = player.second.playerOne;
                        auto playerTwo = player.second.playerTwo;

                        if (playerOne) {
                            playerOne->removeMeAndCleanup();
                        }

                        if (playerTwo) {
                            playerTwo->removeMeAndCleanup();
                        }

                        global->simplePlayerHolderList.erase(player.first);
                    }

                    global->playerDataMap.clear();

                    break;
                }
                case ENET_EVENT_TYPE_NONE: {} // idk what this is supposed to be
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

    connect("127.0.0.1", 23973);

    std::thread eventThread(&pollEvent);
    eventThread.detach();

    return true;
}

GEODE_API void GEODE_CALL geode_unload() {
    enet_host_destroy(Global::get()->host);
}