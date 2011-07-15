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
	void exit();
	void new_player(wsa::socket s);
	bool rename(const std::string& oldname, const std::string& newname);
	void addPlayer(player& p);
	void removePlayer(player& p);
	const players_map& players() const {return m_players;}
	player player_by_name(const std::string& name) const;
	critical_section& lock() {return m_game_lock;}
};

namespace cmd
{
	class error : public std::exception
	{
		std::string message;
	public:
		error(std::string m) : message(m) {}
		const char* what() const {return message.c_str();}
	};
	class exitgame : public std::exception
	{
	public:
		exitgame() {}
		const char* what() const {return "cmd::exitgame exception";}
	};
}


void handle_command(const std::string& name, player_struct& p, game& g, std::istream& params);

