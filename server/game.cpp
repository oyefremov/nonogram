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
		std::cout << "New player connected " << p->name() << std::endl;
		p->game()->addPlayer(p);

		p->socket() << "You was successfully connected to the game as " << p->name() << "\r\n";
		p->main_messages_loop();

		std::cout << "Player " << p->name() << " disconnected" << std::endl;
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
		std::string command, line;
		bool run = true;
		while(run)
		{
			socket() >> line;
			std::stringstream stream(line);
			stream >> command;
			std::cout << name() << ": " << command << std::endl;

			try
			{
				handle_command(command, *this, *game(), stream);
			}
			catch (cmd::error& error)
			{
				std::cerr << "Error in player " << name() << " command handler. \n"
					"Invalid command " << command << "\n"
					"Error message: " << error.what() << std::endl;
			}
		}
	}
	catch(cmd::exitgame&)
	{
		std::cout << "exit game by " << name() << " player" << std::endl;
		game()->exit();
	}
	catch(std::exception& e)
	{
		std::cout << "Error in " << name() << " player loop: " << e.what() << std::endl;
	}
}


void game::new_player(wsa::socket s)
{
	static int player_counter;
	std::unique_ptr<player_struct> p(new player_struct(this, std::move(s)));

	std::stringstream playername;
	playername << "player" << ++player_counter;
	p->set_name(playername.str());

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

void game::exit()
{
	::exit(0);
}

bool game::rename(const std::string& oldname, const std::string& newname)
{
	player& p = m_players[newname];
	if (p) return false;
	p = m_players[oldname];
	m_players.erase(oldname);
	p->set_name(newname);
	return true;
}
