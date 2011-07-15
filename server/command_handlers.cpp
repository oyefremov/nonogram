#include "command_handlers.h"
#include <map>
#include <cassert>
#include <iostream>
#include <sstream>

std::map<std::string, cmd::command_handler_ptr> g_map;

using namespace cmd;

void cmd::register_new_handler(const char* name, cmd::command_handler_ptr handler)
{
	command_handler_ptr& old_handler = g_map[name];
	assert(old_handler == 0);
	old_handler = handler;
}

void cmd::unregister_handler(const char* name)
{
	 auto pos = g_map.find(name);	
	 assert(pos != g_map.end());
	 g_map.erase(pos);
}

void handle_command(const std::string& name, player_struct& p, game& g, std::istream& params)
{
	 auto pos = g_map.find(name);	
	 if (pos != g_map.end())
	 {
		 return pos->second(name, p, g, params);
	 }
	 else
	 {
		 std::stringstream s;
		 s << "Unsupported command " << name << "\r\n";
		 // default handler
		 p.socket() << s.str();
		 throw cmd::error(s.str());
	 }
}
