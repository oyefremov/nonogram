#pragma once
#include <socket\socket.h>
#include "game.h"

void handle_client_request(wsa::socket client, game& g);