#pragma once

#include "game.h"

// Use following macros for defining command handlers
// Sample command deinition
//
// COMMAND(yo)
// {
//		for (size_t i = 0; i < g.players().size(); ++i)
//		{
//			if (g.players()[i] != p)
//				g.players()->socket() << "Yo!";
//		}
//		return cmd::ok;
// }
#define COMMAND(name) \
	cmd::result command_hadler_##name##_hidden(const std::string& name, player_struct& p, game& g);\
	register_handler command_hadler_##name##_hidden_registrator(#name, &command_hadler_##name##_hidden);\
	cmd::result command_hadler_##name##_hidden(const std::string& name, player_struct& p, game& g)

namespace cmd
{
	typedef result (*command_handler_ptr)(const std::string& name, player_struct& p, game& g);

	void register_new_handler(const char* name, command_handler_ptr);
	void unregister_handler(const char* name);

	struct register_handler{
		register_handler(const char* name, command_handler_ptr handler){register_new_handler(name, handler);}
	};
}

