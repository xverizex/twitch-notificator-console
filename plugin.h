#pragma once
#include "shared.h"
#include <zlc_config/config.h>

/*
 * @server - You can select what you want to track single server or set *server to N_SERVERS if you want to track all servers.
 * This is as Twitch server, while it is one server but maybe I add some others servers such as youtube, wasd and etc.
 */
void plugin_init (int *server, struct zl_config *cfg);

/*
 * This is occur when is joined new user.
 */
void plugin_receive_new_join (enum SERVERS server, const char *nick, const char *room);

/*
 * This is occur when is being new message.
 */
void plugin_receive_new_message (enum SERVERS server, const char *room, const char *nick, const char *message);

/*
 * Get plugin name
 */
const char *plugin_get_name ();
