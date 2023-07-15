#include <stdio.h>
#include <iostream>

#include <shared/utilities/Logger.h>
#include <shared/network/TCPServer.h>
#include <shared/network/ASIOThreadPool.h>
#include "network/LoginConnection.h"
#include "network/LoginPacketProcessor.h"
#include "configuration/LoginServerConfiguration.h"
#include "database/LoginDatabases.h"

int main(int /* argc */, char** /* argv */)
{
#ifdef WIN32
	// Activate support for VT100 control characters.
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode;
	GetConsoleMode(hStdOut, &dwMode);
	dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hStdOut, dwMode);
#endif

	sLogger.Info("AION Login Server");

	srand((uint32_t)time(NULL));

	try
	{
		// Load Configuration.
		if (!loginserver::configuration::sLoginServerConfiguration.Load())
		{
			throw std::runtime_error("Failed to load configuration, cannot continue ...");
		}

		// Connect to databases.
		if (!loginserver::database::sLoginDatabases.Initialize())
		{
			throw std::runtime_error("Failed to connect to databases, cannot continue ...");
		}

		// Network Initialization.
		// Start ASIO Thread Pool.
		shared::network::ASIOThreadPool::GetInstance()->SetThreadCount(1);
		shared::network::ASIOThreadPool::GetInstance()->Start();

		// Start Login Packet Processor.
		loginserver::network::sLoginPacketProcessor.Start();

		// Start Login TCP Server.
		shared::network::TCPServer oTCPServer("127.0.0.1", 2106);
		oTCPServer.SetConnectionPrototype(new loginserver::network::LoginConnection());
		oTCPServer.Start();

		sLogger.Info("Press Enter to exit ...");
		std::cin.ignore();

		// Stop TCP Server.
		oTCPServer.Stop();

		// Stop Packet Processor.
		loginserver::network::sLoginPacketProcessor.Stop();

		// Stop ASIO Thread Pool.
		shared::network::ASIOThreadPool::GetInstance()->Stop();

		// Disconnect from databases.
		loginserver::database::sLoginDatabases.Shutdown();

		sLogger.Info("Execution finished.");

		return 0;
	}
	catch (std::exception& rException)
	{
		sLogger.Error("Main Exception raised: %s", rException.what());

		return 1;
	}
}