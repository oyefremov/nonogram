#include "client_handler.h"

void handle_client_request(wsa::socket client, game& g)
{
	g.new_player(std::move(client));
}