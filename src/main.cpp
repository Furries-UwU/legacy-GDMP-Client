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

	ServerPlayerData serverPlayerData = {
		gameManager->getPlayerFrame(),
		gameManager->getPlayerShip(),
		gameManager->getPlayerBall(),
		gameManager->getPlayerBird(),
		gameManager->getPlayerDart(),
		gameManager->getPlayerRobot(),
		gameManager->getPlayerSpider(),
		gameManager->getPlayerGlow(),
		gameManager->getPlayerColor(),
		gameManager->getPlayerColor2()};

	Packet(PLAYER_DATA, sizeof(serverPlayerData), reinterpret_cast<uint8_t *>(&serverPlayerData))
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
		SimplePlayerHolder holder = global.playerObjectHolderList[playerId];

		if (holder.playerOne)
			holder.playerOne->removeFromParent();
		if (holder.playerTwo)
			holder.playerTwo->removeFromParent();

		global.playerDataList.erase(playerId);
		global.playerObjectHolderList.erase(playerId);

		break;
	}
	case PLAYER_JOIN_LEVEL:
	{
		Global global = Global::get();
		auto objectLayer = GameManager::sharedState()->getPlayLayer()->getObjectLayer();

		PlayerJoinLevel playerJoinLevel = *reinterpret_cast<PlayerJoinLevel *>(recievedPacket.data);

		SimplePlayer *player1 = SimplePlayer::create(1);
		player1->updatePlayerFrame(1, IconType::Cube);

		SimplePlayer *player2 = SimplePlayer::create(1);
		player2->updatePlayerFrame(1, IconType::Cube);

		objectLayer->addChild(player1);
		objectLayer->addChild(player2);

		global.playerObjectHolderList[playerJoinLevel.playerId].playerOne = player1;
		global.playerObjectHolderList[playerJoinLevel.playerId].playerTwo = player2;

		break;
	}
	case UPDATE_PLAYER_RENDER_DATA:
	{
		PlayerRenderData renderData = *reinterpret_cast<PlayerRenderData *>(recievedPacket.data);

		GameManager *gm = GameManager::sharedState();

		Global global = Global::get();
		SimplePlayerHolder holder = global.playerObjectHolderList[renderData.playerId];

		SimplePlayer *player1 = holder.playerOne;
		SimplePlayer *player2 = holder.playerTwo;

		if (player1 == NULL || player2 == NULL)
			break;
		


		if (!renderData.visible)
		{
			player1->setVisible(false);
			player2->setVisible(false);
		}
		else if (!renderData.dual)
		{
			player1->setVisible(true);
			player2->setVisible(false);
		}
		else
		{
			player1->setVisible(true);
			player2->setVisible(true);
		}

		fmt::print("X: {}\nY: {}\nScale: {}", renderData.playerOne.posX, renderData.playerOne.posY, renderData.playerOne.scale);

		player1->setPosition(CCPoint(renderData.playerOne.posX, renderData.playerOne.posY));
		player1->setRotation(renderData.playerOne.rotation);
		player1->setScale(renderData.playerOne.scale);

		player2->setPosition(CCPoint(renderData.playerTwo.posX, renderData.playerTwo.posY));
		player2->setRotation(renderData.playerTwo.rotation);
		player2->setScale(renderData.playerTwo.scale);
		
		break;
	}
	case UPDATE_PLAYER_DATA:
	{
		/*
		ClientPlayerData clientPlayerData =
			*reinterpret_cast<ClientPlayerData *>(recievedPacket.data);
		Global::get().playerDataList[clientPlayerData.playerId] = {
			// TODO: Find a better way
			clientPlayerData.cube,
			clientPlayerData.ship, clientPlayerData.ball,
			clientPlayerData.bird, clientPlayerData.dart,
			clientPlayerData.robot, clientPlayerData.spider,
			clientPlayerData.glow, clientPlayerData.primaryColor,
			clientPlayerData.secondaryColor};
			*/
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