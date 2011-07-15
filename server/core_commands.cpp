#include "command_handlers.h"
#include <sstream>

using namespace cmd;

COMMAND(name)
{
	std::string n;
	params >> n;
	if (!g.rename(p.name(), n))
		throw cmd::error("Player already exists");
}

COMMAND(players)
{
	std::stringstream s;
	s << "players:";
	for (auto i = g.players().begin(); i != g.players().end(); ++i)
		s << " " << i->first;
	s << "\r\n";
	p.socket() << s.str();
}

COMMAND(say)
{
	std::string destination_player_name;
	std::string message;
	params >> destination_player_name;
	std::getline(params, message);
	
	player destination = g.player_by_name(destination_player_name);
	if (destination)
	{
		std::stringstream s;
		s << "message from " << p.name() << ": " << message << "\r\n";
		destination->socket() << s.str();
	}
	else
	{
		std::stringstream s;
		s << "Player " << destination_player_name << " not found\r\n";
		p.socket() << s.str();
		throw cmd::error(s.str());
	}
}

COMMAND(yo)
{
	std::string destination_player_name;
	std::string message;
	params >> destination_player_name;
	std::getline(params, message);
	
	for (auto i = g.players().cbegin(); i != g.players().cend(); ++i)
		i->second->socket() << "yo!\r\n";
}

COMMAND(saytoall)
{
	std::string destination_player_name;
	std::string message;
	std::getline(params, message);
	message += "\r\n";
	
	for (auto i = g.players().cbegin(); i != g.players().cend(); ++i)
		if (i->second.get() != &p)
			i->second->socket() << message;
}

COMMAND(exitgame)
{
	throw cmd::exitgame();
}

