#include "dllmain.hpp"

using json = nlohmann::json;

ENetPeer* peer;

void WINAPI SendPlayerData() {
    auto gameManager = GameManager::sharedState();
    auto playerName = gameManager->m_sPlayerName;

    auto serverPlayerData = ServerPlayerData();

    serverPlayerData.username = playerName.c_str();
    serverPlayerData.ship = gameManager->getPlayerShip();
    serverPlayerData.ball = gameManager->getPlayerBall();
    serverPlayerData.bird = gameManager->getPlayerBird();
    serverPlayerData.dart = gameManager->getPlayerDart();
    serverPlayerData.robot = gameManager->getPlayerRobot();
    serverPlayerData.spider = gameManager->getPlayerSpider();
    serverPlayerData.glow = gameManager->getPlayerGlow();
    serverPlayerData.color = gameManager->getPlayerColor();
    serverPlayerData.color2 = gameManager->getPlayerColor2();

    const std::string playerDataJson = json(serverPlayerData).dump();

    Packet(PLAYER_DATA, playerDataJson.length() + 1, reinterpret_cast<uint8_t*>((char*)playerDataJson.c_str())).send(peer);
}

void WINAPI OnRecievedPacket(ENetPeer* peer, ENetEvent event) {

    Packet recievedPacket = Packet(event.packet);
    switch (recievedPacket.type)
    {
    case 0x01: {
        break;
    }
    }
}

void WINAPI eventThread(LPVOID lpParam) {
    if (enet_initialize() != 0)
    {
        fmt::print("An error occurred while initializing ENet.\n");
        return;
    }
    atexit(enet_deinitialize);

    ENetHost* client;
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

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 23973;

    peer = enet_host_connect(client, &address, 1, 0);

    if (peer == NULL)
    {
        fmt::print("No available peers for initiating an ENet connection.");
        exit(EXIT_FAILURE);
    }

    Global::get().peer = peer;

    while (true)
    {
        ENetEvent event;

        while (enet_host_service(client, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT: {
                SendPlayerData();
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                OnRecievedPacket(peer, event);

                enet_packet_destroy(event.packet);
                break;
            }
            }
        }
    }

    enet_host_destroy(client);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason != DLL_PROCESS_ATTACH)
        return FALSE;

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

    if (MH_Initialize() != MH_OK) {
        fmt::print("An error occurred while initializing Minhook.\n");
        return FALSE;
    }

    CreateThread(NULL, 0x1000, reinterpret_cast<LPTHREAD_START_ROUTINE>(&eventThread), NULL, 0, NULL);

    createHook();
    MH_EnableHook(MH_ALL_HOOKS);

    return TRUE;
}