#include <stdio.h>
#include <iostream>

#include <shared/network/TCPServer.h>
#include <shared/network/ASIOThreadPool.h>
#include "network/LoginConnection.h"

int main(int argc, char* argv[])
{
	printf("AION Login Server\n");

	try
	{
		shared::ASIOThreadPool::GetInstance()->SetThreadCount(4);
		shared::ASIOThreadPool::GetInstance()->Start();

		shared::TCPServer oTCPServer("127.0.0.1", 2107);
		oTCPServer.SetConnectionPrototype(new LoginConnection());
		oTCPServer.Start();

		printf("Press Enter to exit ...\n");
		std::cin.ignore();

		oTCPServer.Stop();

		shared::ASIOThreadPool::GetInstance()->Stop();

		printf("Execution finished.\n");

		return 0;
	}
	catch (std::exception& rException)
	{
		printf("Main Exception raised: %s\n", rException.what());
		return 1;
	}

	

	
	

	return 0;
}