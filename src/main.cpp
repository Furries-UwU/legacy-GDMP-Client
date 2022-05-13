#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAdress, int port)
{
	ENetAddress address;

	enet_address_set_host(&address, ipAdress);
	address.port = port;

	Global::get().peer = enet_host_connect(Global::get().host, &address, 1, 0);
}

void SendPlayerData()
{
	auto gameManager = GameManager::sharedState();

	ServerPlayerData serverPlayerData = ServerPlayerData();
	serverPlayerData.ship = gameManager->getPlayerShip();
	serverPlayerData.ball = gameManager->getPlayerBall();
	serverPlayerData.bird = gameManager->getPlayerBird();
	serverPlayerData.dart = gameManager->getPlayerDart();
	serverPlayerData.robot = gameManager->getPlayerRobot();
	serverPlayerData.spider = gameManager->getPlayerSpider();
	serverPlayerData.glow = gameManager->getPlayerGlow();
	serverPlayerData.primaryColor = gameManager->getPlayerColor();
	serverPlayerData.secondaryColor = gameManager->getPlayerColor2();

	Packet(PLAYER_DATA, sizeof(serverPlayerData),
		   reinterpret_cast<uint8_t *>(&serverPlayerData))
		.send(Global::get().peer);
}

void OnRecievedPacket(ENetPeer *peer, ENetEvent event)
{

	Packet recievedPacket = Packet(event.packet);
	switch (recievedPacket.type)
	{
	case PLAYER_LEAVE_LEVEL:
	{
		unsigned int playerId =
			*reinterpret_cast<unsigned int *>(recievedPacket.data);

		Global global = Global::get();
		PlayerObjectHolder holder = global.playerObjectHolderList[playerId];

		if (holder.playerOne)
			holder.playerOne->removeFromParent();
		if (holder.playerTwo)
			holder.playerTwo->removeFromParent();

		global.playerDataList.erase(playerId);
		global.playerObjectHolderList.erase(playerId);
	}
	case UPDATE_PLAYER_DATA:
	{
		ClientPlayerData clientPlayerData =
			*reinterpret_cast<ClientPlayerData *>(recievedPacket.data);
		Global::get().playerDataList[clientPlayerData.playerId] = {
			// TODO: Find a better way
			clientPlayerData.ship, clientPlayerData.ball,
			clientPlayerData.bird, clientPlayerData.dart,
			clientPlayerData.robot, clientPlayerData.spider,
			clientPlayerData.glow, clientPlayerData.primaryColor,
			clientPlayerData.secondaryColor};
		break;
	}
	}
}

// PLEASE RUN THIS IN ANOTHER THREAD
void pollEvent()
{
	while (true)
	{
		ENetHost *host = Global::get().host;
		ENetEvent event;

		while (enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				SendPlayerData();
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				OnRecievedPacket(event.peer, event);
				enet_packet_destroy(event.packet);
				break;
			}
		}
	}
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

	Global::get().host = host;

	connect("127.0.0.1", 23973);

	std::thread eventThread(&pollEvent);
	eventThread.detach();

	return true;
}

GEODE_API void GEODE_CALL geode_unload()
{
	enet_host_destroy(Global::get().host);
	enet_deinitialize();
}