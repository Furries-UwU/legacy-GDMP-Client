#include "main.hpp"

USE_GEODE_NAMESPACE();

void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *statusInfo)
{
	if (statusInfo->m_hConn != m_hConnection && m_hConnection != k_HSteamNetConnection_Invalid) return;

	switch (statusInfo->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_None:
		break;

	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
	{
		Global::get()->interface->CloseConnection(statusInfo->m_hConn, 0, nullptr, false);
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
	option.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)SteamNetConnectionStatusChangedCallback);

	global->connection = global->interface->ConnectByIPAddress(serverAddress, 1, &option);

	if (global->connection == k_HSteamNetConnection_Invalid)
		fmt::print("Failed to create connection");
}

void SendPlayerData()
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
		.send(Global::get()->interface, Global::get()->connection);
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

	incomingMessage->Release();
}

// PLEASE RUN THIS IN ANOTHER THREAD
void pollEvent()
{
	while (true)
	{
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

	Global::get()->interface = SteamNetworkingSockets();

	connect("127.0.0.1", 23973);

	std::thread eventThread(&pollEvent);
	eventThread.detach();

	return true;
}

GEODE_API void GEODE_CALL geode_unload()
{
	GameNetworkingSockets_Kill();
}