#pragma once

#include <socket\socket.h>
#include <vector>
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

	void setName(std::string n) {m_name = n;}
	const std::string& getName() const {return m_name;}
};

typedef std::shared_ptr<player_struct> player;

typedef std::vector<player> players_vector;
class game
{
	players_vector m_players;
	critical_section m_game_lock;
public:
	~game();
	void new_player(wsa::socket s);
	void addPlayer(player& p);
	void removePlayer(player& p);
	const players_vector& players() const {return m_players;}
	critical_section& lock() {return m_game_lock;}
};