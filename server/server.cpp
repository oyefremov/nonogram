// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <socket\socket.h>
#include "game.h"

int main(int argc, char* argv[])
{
	try
	{
		wsa::WSAInitialize();

		wsa::server_socket server = wsa::listen("127.0.0.1", 12345);

		game my_game;

		for(;;)
		{
			my_game.new_player(server.accept());
		}

		return 0;
	}
   	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}

