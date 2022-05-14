#include "main.hpp"

USE_GEODE_NAMESPACE();

void connect(char *ipAdress, int port)
{
	ENetAddress address;

	enet_address_set_host(&address, ipAdress);
	address.port = port;

	Global::get()->peer = enet_host_connect(Global::get()->host, &address, 1, 0);
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
		.send(Global::get()->peer);
}

void updateRender(SimplePlayer *simplePlayer, ServerPlayerData playerData, BaseRenderData renderData)
{
	IconType iconType = Utility::getIconType(renderData);

	simplePlayer->updatePlayerFrame(Utility::getIconId(iconType, playerData), iconType);
	simplePlayer->setPosition({renderData.posX, renderData.posY});
	simplePlayer->setRotation(renderData.rotation);
	simplePlayer->setScale(renderData.scale);
}

void updateSkin(SimplePlayer *simplePlayer, ServerPlayerData playerData, bool swapColor = false)
{
	GameManager *gm = GameManager::sharedState();

	_ccColor3B primaryColor = gm->colorForIdx(playerData.primaryColor);
	_ccColor3B secondaryColor = gm->colorForIdx(playerData.secondaryColor);

	simplePlayer->setColor(swapColor ? secondaryColor : primaryColor);
	simplePlayer->setSecondColor(swapColor ? primaryColor : secondaryColor);
	simplePlayer->updateColors();
	simplePlayer->setGlowOutline(playerData.glow);
}

void OnRecievedPacket(ENetEvent event)
{

	Packet recievedPacket = Packet(event.packet);

	fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", event.packet->dataLength, recievedPacket.type, recievedPacket.length);
	for (int x = 0; x < event.packet->dataLength; x++)
	{
		fmt::print(" {:#04x}", recievedPacket[x]);
	}
	fmt::print("\n\n");

	switch (recievedPacket.type)
	{
	case PLAYER_LEAVE_LEVEL:
	{
		unsigned int playerId =
			*reinterpret_cast<unsigned int *>(recievedPacket.data);

		Global* global = Global::get();
		SimplePlayerHolder holder = global->simplePlayerObjectHolderList[playerId];

		if (holder.playerOne)
			holder.playerOne->removeMeAndCleanup();
		if (holder.playerTwo)
			holder.playerTwo->removeMeAndCleanup();

		global->playerDataList.erase(playerId);
		global->simplePlayerObjectHolderList.erase(playerId);

		break;
	}
	case PLAYER_JOIN_LEVEL:
	{
		Global* global = Global::get();
		auto playLayer = GameManager::sharedState()->getPlayLayer();

		if (!playLayer) 
			break;

		auto objectLayer = playLayer->getObjectLayer();

		unsigned int playerId =
			*reinterpret_cast<unsigned int *>(recievedPacket.data);

		ServerPlayerData serverPlayerData = global->playerDataList[playerId];

		SimplePlayer *player1 = SimplePlayer::create(global->playerDataList[playerId].cube);
		player1->updatePlayerFrame(Utility::getIconId(IconType::Cube, serverPlayerData), IconType::Cube);

		SimplePlayer *player2 = SimplePlayer::create(global->playerDataList[playerId].cube);
		player2->updatePlayerFrame(Utility::getIconId(IconType::Cube, serverPlayerData), IconType::Cube);
		player2->setVisible(false);

		updateSkin(player1, serverPlayerData);
		updateSkin(player2, serverPlayerData, true);

		objectLayer->addChild(player1);
		objectLayer->addChild(player2);

		global->simplePlayerObjectHolderList[playerId].playerOne = player1;
		global->simplePlayerObjectHolderList[playerId].playerTwo = player2;

		break;
	}
	case UPDATE_PLAYER_RENDER_DATA:
	{
		PlayerRenderData renderData = *reinterpret_cast<PlayerRenderData *>(recievedPacket.data);

		GameManager *gm = GameManager::sharedState();

		Global* global = Global::get();
		SimplePlayerHolder holder = global->simplePlayerObjectHolderList[renderData.playerId];

		SimplePlayer *player1 = holder.playerOne;
		SimplePlayer *player2 = holder.playerTwo;

		ServerPlayerData serverPlayerData = global->playerDataList[renderData.playerId];

		if (player1)
		{
			updateSkin(player1, serverPlayerData);
			updateRender(player1, serverPlayerData, renderData.playerOne);

			player1->setVisible(renderData.visible);
		}

		if (player2)
		{
			updateSkin(player2, serverPlayerData, true);
			updateRender(player2, serverPlayerData, renderData.playerTwo);

			player2->setVisible(renderData.dual);
		}

		break;
	}
	case UPDATE_PLAYER_DATA:
	{
		ClientPlayerData clientPlayerData =
			*reinterpret_cast<ClientPlayerData *>(recievedPacket.data);
		Global::get()->playerDataList[clientPlayerData.playerId] = {
			// TODO: Find a better way
			clientPlayerData.cube,
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
		ENetHost *host = Global::get()->host;
		ENetEvent event;

		while (enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_DISCONNECT:
				for (auto v : Global::get()->simplePlayerObjectHolderList)
				{
					v.second.playerOne->removeMeAndCleanup();
					v.second.playerTwo->removeMeAndCleanup();
				};

				Global::get()->playerDataList.clear();
				Global::get()->simplePlayerObjectHolderList.clear();
				break;
			case ENET_EVENT_TYPE_CONNECT:
				fmt::print("Connected!");
				SendPlayerData();
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				Global::get()->queueInGDThread(std::bind(OnRecievedPacket, event));
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

	Global::get()->host = host;

	connect("144.91.110.176", 23973);

	std::thread eventThread(&pollEvent);
	eventThread.detach();

	return true;
}

GEODE_API void GEODE_CALL geode_unload()
{
	enet_host_destroy(Global::get()->host);
	enet_deinitialize();
}