#include "command_handlers.h"
#include <sstream>

using namespace cmd;

COMMAND(players)
{
	std::stringstream s;
	s << "players" << std::endl;
	for (auto i = g.players().begin(); i != g.players().end(); ++i)
		s << i->first << std::endl;
	p.socket() << s.str();
	return cmd::ok;
}

COMMAND(say)
{
	std::string destination_player_name;
	std::string message;
	p.socket() >> destination_player_name >> message;
	
	player destination = g.player_by_name(destination_player_name);
	if (destination)
	{
		std::stringstream s;
		s << "message" << p.name() << message;
		destination->socket() << s.str();
		return cmd::ok;
	}
	else
	{
		std::stringstream s;
		s << "Player " << destination_player_name << " not found";
		p.socket() << s.str();
		return cmd::error;
	}
}

COMMAND(new1)
{
	std::string destination_player_name;
	std::string message;
	p.socket() >> destination_player_name >> message;
	
	player destination = g.player_by_name(destination_player_name);
	if (destination)
	{
		std::stringstream s;
		s << "message" << p.name() << message;
		destination->socket() << s.str();
		return cmd::ok;
	}
	else
	{
		std::stringstream s;
		s << "Player " << destination_player_name << " not found";
		p.socket() << s.str();
		return cmd::error;
	}
}