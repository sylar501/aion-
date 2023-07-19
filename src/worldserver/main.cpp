#include <stdio.h>
#include <iostream>

#include <shared/utilities/Logger.h>
#include <shared/network/TCPServer.h>
#include <shared/network/ASIOThreadPool.h>
#include "network/WorldConnection.h"
#include "network/WorldPacketProcessor.h"
#include "configuration/WorldServerConfiguration.h"

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

	sLogger.Info("AION World Server");

	srand((uint32_t)time(NULL));

	try
	{
		// Load Configuration.
		if (!worldserver::configuration::sWorldServerConfiguration.Load())
		{
			throw std::runtime_error("Failed to load configuration, cannot continue ...");
		}

		// Network Initialization.
		// Start ASIO Thread Pool.
		shared::network::ASIOThreadPool::GetInstance()->SetThreadCount(1);
		shared::network::ASIOThreadPool::GetInstance()->Start();

		// Start Login Packet Processor.
		worldserver::network::sWorldPacketProcessor.Start();

		// Start Login TCP Server.
		shared::network::TCPServer oTCPServer("127.0.0.1", 10250);
		oTCPServer.SetConnectionPrototype(new worldserver::network::WorldConnection());
		oTCPServer.Start();

		sLogger.Info("Press Enter to exit ...");
		std::cin.ignore();

		oTCPServer.Stop();

		worldserver::network::sWorldPacketProcessor.Stop();

		shared::network::ASIOThreadPool::GetInstance()->Stop();

		sLogger.Info("Execution finished.");

		return 0;
	}
	catch (std::exception& rException)
	{
		sLogger.Error("Main Exception raised: %s", rException.what());

		return 1;
	}
}