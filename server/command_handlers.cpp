#include "command_handlers.h"
#include <map>
#include <cassert>
#include <iostream>

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

cmd::result handle_command(const std::string& name, player_struct& p, game& g)
{
	 auto pos = g_map.find(name);	
	 if (pos != g_map.end())
	 {
		 return pos->second(name, p, g);
	 }
	 else
	 {
		 // default handler
		 p.socket() << "Unsupported command " << name;
		 return cmd::error;
	 }
}
