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
		shared::ASIOThreadPool::GetInstance()->SetThreadCount(4);
		shared::ASIOThreadPool::GetInstance()->Start();

		shared::TCPServer oTCPServer("127.0.0.1", 2107);
		oTCPServer.SetConnectionPrototype(new LoginConnection());
		oTCPServer.Start();

		sLogger.Info("Press Enter to exit ...");
		std::cin.ignore();

		oTCPServer.Stop();

		shared::ASIOThreadPool::GetInstance()->Stop();

		sLogger.Info("Execution finished.");

		return 0;
	}
	catch (std::exception& rException)
	{
		sLogger.Error("Main Exception raised: %s", rException.what());

		return 1;
	}
}