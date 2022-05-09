#include "dllmain.hpp"

#pragma pack(push, 1)
struct Packet {
    uint8_t type;
    uint8_t* data;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerData {
    char* username;
    int ship;
	int ball;
    int bird;
    int dart;
    int robot;
    int spider;
    int glow;
    int color;
    int color2;
};
#pragma pack(pop)

void sendPacket(ENetPeer* peer, Packet data) {
    ENetPacket* packet = enet_packet_create(nullptr,
        sizeof(data),
        ENET_PACKET_FLAG_RELIABLE);

    std::memcpy(packet->data, &data, sizeof(data));

    if (enet_peer_send(peer, 0, packet) != 0)
        enet_packet_destroy(packet);
}

DWORD MainThread(LPVOID lpParam)
{
#ifdef SHOW_DEBUG_CONSOLE
    AllocConsole();
    SetConsoleTitle(LPCSTR("Geometry Dash"));

    // Redirect cout, cin and cerr
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$", "r", stdin);
#endif

    if (enet_initialize() != 0)
    {
        fmt::print("An error occurred while initializing ENet.\n");
        return FALSE;
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
    ENetPeer* peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 23973;

    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        fmt::print("No available peers for initiating an ENet connection.");
        exit(EXIT_FAILURE);
    }

    while (true) {
        ENetEvent event;

        while (enet_host_service(client, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                Packet packet = *reinterpret_cast<Packet*>(event.packet->data);
				
                switch (packet.type) {
                    case 0x01: {
                        auto gameManager = gd::GameManager::sharedState();
						auto playerName = gameManager->m_sPlayerName;

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

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason != DLL_PROCESS_ATTACH) return FALSE;
	
    CreateThread(NULL, 0x1000, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MainThread), NULL, 0, NULL);

    return TRUE;
}