#include "dllmain.hpp"

using json = nlohmann::json;

DWORD WINAPI MainThread(LPVOID lpParam)
{
#ifdef DEBUG
    if (AllocConsole()) {
        SetConsoleTitle(LPCSTR("Geometry Dash"));

        // Redirect cout, cin and cerr
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$", "r", stdin);
    }   
#endif

    if (enet_initialize() != 0)
    {
        fmt::print("An error occurred while initializing ENet.\n");
        return FALSE;
    }
    atexit(enet_deinitialize);

    ENetHost *client;
    client = enet_host_create(NULL,
                              1,
                              1,
                              0,
                              0);

    if (client == NULL)
    {
        fmt::print("An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    ENetPeer *peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 23973;

    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        fmt::print("No available peers for initiating an ENet connection.");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        ENetEvent event;

        while (enet_host_service(client, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                Packet packet = *reinterpret_cast<Packet *>(event.packet->data);

                switch (packet.type)
                {
                case 0x01:
                {
                    auto gameManager = gd::GameManager::sharedState();
                    auto playerName = gameManager->m_sPlayerName;

                    auto playerData = PlayerData();

					playerData.username = playerName.c_str();
                    playerData.ship = gameManager->getPlayerShip();
                    playerData.ball = gameManager->getPlayerBall();
                    playerData.bird = gameManager->getPlayerBird();
                    playerData.dart = gameManager->getPlayerDart();
                    playerData.robot = gameManager->getPlayerRobot();
                    playerData.spider = gameManager->getPlayerSpider();
                    playerData.glow = gameManager->getPlayerGlow();
                    playerData.color = gameManager->getPlayerColor();
                    playerData.color2 = gameManager->getPlayerColor2();

                    const char* playerDataJson = json(playerData).dump().c_str();

                    const auto packet = Packet(0x01, reinterpret_cast<uint8_t*>(&playerDataJson));

                    sendPacket(peer, packet, sizeof(packet));
                    break;
                }
                }

                break;
            }
            }
        }
    }

    enet_peer_reset(peer);
    enet_host_destroy(client);

    return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason != DLL_PROCESS_ATTACH)
        return FALSE;

    CreateThread(NULL, 0x1000, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MainThread), NULL, 0, NULL);

    return TRUE;
}