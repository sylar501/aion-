#include <stdio.h>
#include <iostream>

#include <shared/utilities/Logger.h>
#include <shared/network/TCPServer.h>
#include <shared/network/ASIOThreadPool.h>
#include "network/LoginConnection.h"

int main(int /* argc */, char** /* argv */)
{
	sLogger.Info("AION Login Server");

	srand((uint32_t)time(NULL));

	try
	{
		shared::network::ASIOThreadPool::GetInstance()->SetThreadCount(1);
		shared::network::ASIOThreadPool::GetInstance()->Start();

		shared::network::TCPServer oTCPServer("127.0.0.1", 2106);
		oTCPServer.SetConnectionPrototype(new LoginConnection());
		oTCPServer.Start();

		sLogger.Info("Press Enter to exit ...");
		std::cin.ignore();

		oTCPServer.Stop();

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