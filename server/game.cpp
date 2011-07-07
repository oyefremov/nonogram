#include "game.h"
#include <iostream>
#include <sstream>

game::~game()
{
}

DWORD WINAPI NewPlayerHandler(LPVOID lpParameter)
{
	player p(reinterpret_cast<player_struct*>(lpParameter));
	try
	{
		std::cout << "New player connected ";
		std::string name;
		p->socket() >> name;

		std::cout << name << std::endl;
		p->set_name(name);
		p->game()->addPlayer(p);

		p->socket() << "You was successfully connected to the game";
		p->main_messages_loop();

		std::cout << "Player " << name << " disconnected" << std::endl;
	}
	catch(std::exception& e)
	{
		std::cout << "Error " << e.what() << std::endl;
	}
	p->game()->removePlayer(p);
	return 0;
}

void player_struct::main_messages_loop()
{
	try
	{
		std::string command;
		bool run = true;
		while(run)
		{
			socket() >> command;
			std::cout << name() << ": " << command << std::endl;

			switch(handle_command(command, *this, *game()))
			{
			case cmd::ok:
				break;
			case cmd::error:
				std::cerr << "Error in player " << name() << " command handler. Invalid command " << command << std::endl;
				break;
			case cmd::logout_player:
				run = false;
				break;
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Error in " << name() << " player loop: " << e.what() << std::endl;
	}
}


void game::new_player(wsa::socket s)
{
	std::unique_ptr<player_struct> p(new player_struct(this, std::move(s)));
	HANDLE h = CreateThread(0, 0, &NewPlayerHandler, p.get(), CREATE_SUSPENDED, 0);
	if (h == 0)
		WinAPIError("Error in CreateThread");
	p->setThreadHandle(h);
	DWORD res = ResumeThread(h);
	if (res == -1)
		WinAPIError("Error in ResumeThread");

	p.release();
}

void game::addPlayer(player& p)
{
	synch s(m_game_lock);
	m_players[p->name()] = p;
}

void game::removePlayer(player& p)
{
	synch s(m_game_lock);
	m_players.erase(m_players.find(p->name()));
}

player game::player_by_name(const std::string& name) const
{
	auto pos = m_players.find(name);
	if (pos == m_players.end()) return player();
	return pos->second;
}