#pragma once

#include <socket\socket.h>
#include <game\nonogram.h>
#include <vector>
#include <map>
#include <algorithm>
#include "mutex.h"

class game;


class player_struct
{
	wsa::socket m_socket;
	game* m_game; 
	HANDLE m_thread;
	std::string m_name;
	player_struct(player_struct&);

public:
	player_struct(game* g, wsa::socket s) : m_game(g), m_socket(std::move(s)) {}

	void setThreadHandle(HANDLE h) {m_thread = h;}
	game* game() {return m_game;}
	wsa::socket& socket() {return m_socket;}

	void main_messages_loop();

	void set_name(std::string n) {m_name = n;}
	const std::string& name() const {return m_name;}
};

typedef std::shared_ptr<player_struct> player;

typedef std::map<std::string, player> players_map;
class game
{
	players_map m_players;
	nonogram m_game;
	critical_section m_game_lock;
public:
	~game();
	void new_player(wsa::socket s);
	void addPlayer(player& p);
	void removePlayer(player& p);
	const players_map& players() const {return m_players;}
	player player_by_name(const std::string& name) const;
	critical_section& lock() {return m_game_lock;}
};

namespace cmd
{
	enum result
	{
		ok,
		error,
		logout_player
	};
}
cmd::result handle_command(const std::string& name, player_struct& p, game& g);

