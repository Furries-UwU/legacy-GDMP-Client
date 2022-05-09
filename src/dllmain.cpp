#include "dllmain.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason != DLL_PROCESS_ATTACH) return FALSE;

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
        fmt::print("An error occurred while initializing ENet.");
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
        fmt::print("An error occurred while trying to create an ENet client host.");
        exit(EXIT_FAILURE);
    }

    enet_host_destroy(client);

    return TRUE;
}