#include "game.h"
#include <iostream>
#include <sstream>

game::~game()
{
}

void list_players(game& g, wsa::socket& s)
{
	std::stringstream list;
	{
		synch s(g.lock());
		for (size_t i=0; i<g.players().size(); ++i)
			list << g.players()[i]->getName() << std::endl;
	}
	s << list.str().c_str();
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
		p->setName(name);
		p->game()->addPlayer(p);

		p->socket() << "You was successfully connected to the game";
			
		for(;;)
		{
			std::string command;
			p->socket() >> command;
			std::cout << p->getName() << ": " << command << std::endl;
			if (command == "list players")
				list_players(*p->game(), p->socket());
			else p->socket() << "Unknown command";
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Error " << e.what() << std::endl;
	}
	p->game()->removePlayer(p);
	return 0;
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
	m_players.push_back(p);
}

void game::removePlayer(player& p)
{
	synch s(m_game_lock);
	m_players.erase(std::find(m_players.begin(), m_players.end(), p));
}
