#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAdress, int port)
{
	ENetAddress address;

	enet_address_set_host(&address, ipAdress);
	address.port = port;

	Global::get()->peer = enet_host_connect(Global::get()->host, &address, 1, 0);
}

GEODE_API bool GEODE_CALL geode_load(Mod *mod)
{
	if (enet_initialize() != 0)
	{
		fmt::print("An error occurred while initializing ENet.\n");
		return false;
	}

	ENetHost *host;
	host = enet_host_create(NULL, 1, 1, 0, 0);

	if (host == NULL)
	{
		fmt::print(
			"An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}

	Global::get()->host = host;

	connect("127.0.0.1", 23973);

	return true;
}

GEODE_API void GEODE_CALL geode_unload()
{
	enet_host_destroy(Global::get()->host);
	enet_deinitialize();
}