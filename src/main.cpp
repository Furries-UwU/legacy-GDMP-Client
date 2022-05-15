#include "main.hpp"

USE_GEODE_NAMESPACE();

void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *statusInfo)
{
	switch (statusInfo->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_None:
		break;

	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
	{
		Global::get()->nInterface->CloseConnection(statusInfo->m_hConn, 0, nullptr, false);
		Global::get()->connection = k_HSteamNetConnection_Invalid;
		break;
	}

	case k_ESteamNetworkingConnectionState_Connecting:
		break;

	case k_ESteamNetworkingConnectionState_Connected:
		break;

	default:
		// Silences -Wswitch
		break;
	}
}

void connect(char *ipAddress, int port)
{
	Global *global = Global::get();

	SteamNetworkingIPAddr serverAddress;
	serverAddress.Clear();

	serverAddress.ParseString(ipAddress);
	serverAddress.m_port = port;

	SteamNetworkingConfigValue_t option;
	option.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)OnSteamNetConnectionStatusChanged);

	global->connection = global->nInterface->ConnectByIPAddress(serverAddress, 1, &option);

	if (global->connection == k_HSteamNetConnection_Invalid)
		fmt::print("Failed to create connection");
}

void sendColorData()
{
	auto gameManager = GameManager::sharedState();

	IconData iconData = {
		gameManager->getPlayerFrame(),
		gameManager->getPlayerShip(),
		gameManager->getPlayerBall(),
		gameManager->getPlayerBird(),
		gameManager->getPlayerDart(),
		gameManager->getPlayerRobot(),
		gameManager->getPlayerSpider()};

	Packet(ICON_DATA, sizeof(iconData), reinterpret_cast<uint8_t *>(&iconData))
		.send(Global::get()->nInterface, Global::get()->connection);
}

void sendColorData()
{
	auto gameManager = GameManager::sharedState();

	ColorData colorData = {
		gameManager->m_playerColor,
		gameManager->m_playerColor2,
		gameManager->m_playerGlow};

	Packet(COLOR_DATA, sizeof(colorData), reinterpret_cast<uint8_t *>(&colorData))
		.send(Global::get()->nInterface, Global::get()->connection);
}

void updateRender(SimplePlayer *simplePlayer, BaseRenderData renderData)
{
	IconType iconType = Utility::getIconType(renderData);

	simplePlayer->setPosition({renderData.position.x, renderData.position.y});
	simplePlayer->setRotation(renderData.rotation);
	simplePlayer->setScale(renderData.scale);
}

void OnRecievedMessage(ISteamNetworkingMessage *incomingMessage)
{
	Packet recievedPacket = Packet(incomingMessage);

	fmt::print("Host -> Me\nPacket Length: {}\nPacket Type: {}\nPacket's Data Length: {}\nHex:", incomingMessage->m_cbSize, recievedPacket.type, recievedPacket.length);
	for (int x = 0; x < incomingMessage->m_cbSize; x++)
	{
		fmt::print(" {:#04x}", recievedPacket[x]);
	}
	fmt::print("\n\n");

	switch (recievedPacket.type)
	{
	case (RENDER_DATA):
	{
		IncomingRenderData incomingRenderData = *reinterpret_cast<IncomingRenderData *>(recievedPacket.data);

		Global *global = Global::get();
		SimplePlayerHolder playerHolder = global->simplePlayerHolderList[incomingRenderData.playerId];

		if (playerHolder.playerOne)
		{
			updateRender(playerHolder.playerOne, incomingRenderData.renderData.playerOne);
			playerHolder.playerOne->setVisible(incomingRenderData.renderData.isVisible);
		}

		if (playerHolder.playerTwo)
		{
			updateRender(playerHolder.playerTwo, incomingRenderData.renderData.playerTwo);
			playerHolder.playerTwo->setVisible(incomingRenderData.renderData.isDual);
		}

		break;
	}

	case (LEAVE_LEVEL):
	{
		int playerId = *reinterpret_cast<int *>(recievedPacket.data);

		Global *global = Global::get();

		auto playerOne = global->simplePlayerHolderList[playerId].playerOne;
		auto playerTwo = global->simplePlayerHolderList[playerId].playerTwo;

		if (playerOne)
			playerOne->removeMeAndCleanup();

		if (playerTwo)
			playerTwo->removeMeAndCleanup();

		global->simplePlayerHolderList.erase(playerId);
		global->playerDataMap.erase(playerId);
	}

	case (JOIN_LEVEL):
	{
		int playerId = *reinterpret_cast<int *>(recievedPacket.data);

		Global::get()->queueInGDThread([playerId]()
									   {
      Global * global = Global::get();

      auto playLayer = GameManager::sharedState() -> getPlayLayer();

      if (!playLayer)
        return;

      const auto objectLayer = playLayer -> getObjectLayer();

      SimplePlayer * player1 = SimplePlayer::create(1);
      player1 -> updatePlayerFrame(1, IconType::Cube);

      SimplePlayer * player2 = SimplePlayer::create(1);
      player2 -> updatePlayerFrame(1, IconType::Cube);
      player2 -> setVisible(false);

      objectLayer -> addChild(player1);
      objectLayer -> addChild(player2);

      global -> simplePlayerHolderList[playerId].playerOne = player1;
      global -> simplePlayerHolderList[playerId].playerTwo = player2; });

		break;
	}
	}

	incomingMessage->Release();
}

// PLEASE RUN THIS IN ANOTHER THREAD
void pollEvent()
{
	while (true)
	{
		ISteamNetworkingMessage *incomingMessage = nullptr;
		int numMsgs = Global::get()->nInterface->ReceiveMessagesOnPollGroup(Global::get()->pollGroup, &incomingMessage, 1);

		if (numMsgs == 0)
			continue;

		if (numMsgs < 0)
		{
			fmt::print("Error checking for messages");
			return;
		}

		if (incomingMessage->m_cbSize < 5)
		{
			fmt::print("Recieved invalid packet");
			continue;
		}

		OnRecievedMessage(incomingMessage);
	}
}

GEODE_API bool GEODE_CALL geode_load(Mod *mod)
{
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		fmt::print("GameNetworkingSockets_Init failed. {}", errMsg);
		return false;
	}

	Global::get()->nInterface = SteamNetworkingSockets();

	connect("127.0.0.1", 23973);

	std::thread eventThread(&pollEvent);
	eventThread.detach();

	return true;
}

GEODE_API void GEODE_CALL geode_unload()
{
	GameNetworkingSockets_Kill();
}