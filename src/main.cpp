#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAddress, int port) {
    Global *global = Global::get();

    ENetAddress address;
    ENetPeer *peer;

    enet_address_set_host(&address, ipAddress);
    address.port = port;

    peer = enet_host_connect(global->host, &address, 1, 0);
    if (peer == NULL) {
        fmt::print(stderr,
                   "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
}

void updateRender(SimplePlayer *simplePlayer, BaseRenderData renderData) {
    simplePlayer->setPosition({renderData.position.x, renderData.position.y});
    simplePlayer->setRotation(renderData.rotation);
    simplePlayer->setScale(renderData.scale);
}

void OnRecievedMessage(ENetPacket *eNetPacket) {
    if (eNetPacket->dataLength < 5) {
        enet_packet_destroy(eNetPacket);
        return;
    }

    auto packet = Packet::serialize(eNetPacket);

    fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", eNetPacket->dataLength,
               packet.type, packet.length);
    for (int x = 0; x < eNetPacket->dataLength; x++) {
        fmt::print(" {:#04x}", packet[x]);
    }
    fmt::print("\n\n");

    switch (packet.type) {
        case (RENDER_DATA): {
            auto incomingRenderData = json(*reinterpret_cast<uint8_t *>(packet.data),
                                           packet.length).get<IncomingRenderData>();

            Global *global = Global::get();
            SimplePlayerHolder playerHolder = global->simplePlayerHolderList[incomingRenderData.playerId];

            if (playerHolder.playerOne) {
                updateRender(playerHolder.playerOne, incomingRenderData.renderData.playerOne);
                playerHolder.playerOne->setVisible(incomingRenderData.renderData.isVisible);
            }

            if (playerHolder.playerTwo) {
                updateRender(playerHolder.playerTwo, incomingRenderData.renderData.playerTwo);
                playerHolder.playerTwo->setVisible(incomingRenderData.renderData.isDual);
            }

            break;
        }

        case (JOIN_LEVEL): {
            int playerId = *reinterpret_cast<int *>(packet.data);

            Global::get()->queueInGDThread([playerId]() {
                Global *global = Global::get();

                auto playLayer = GameManager::sharedState()->getPlayLayer();

                if (!playLayer)
                    return;

                const auto objectLayer = playLayer->getObjectLayer();

                SimplePlayer *player1 = SimplePlayer::create(1);
                player1->updatePlayerFrame(1, IconType::Cube);

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
                case ENET_EVENT_TYPE_RECEIVE:
                    OnRecievedMessage(event.packet);
                    break;
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
    global->host = enet_host_create(nullptr, 1, 2, 0, 0);

    connect("192.168.1.33", 23973);

    std::thread eventThread(&pollEvent);
    eventThread.detach();

    return true;
}

GEODE_API void GEODE_CALL geode_unload() {
    enet_host_destroy(Global::get()->host);
}