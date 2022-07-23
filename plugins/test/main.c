#include <string.h>
#include <stdio.h>
#include <network.h>
#include <plugin.h>
#include <answers.h>
#include <shared.h>
#include <zlc_config/config.h>
#include <config.h>
#include <stdlib.h>

static struct zl_config *main_cfg;

void plugin_init (int *server, struct zl_config *cfg) {
	*server = N_SERVERS;
	main_cfg = cfg;
}

void plugin_receive_new_join (enum SERVERS server, const char *nick, const char *room) {
	
}

void plugin_receive_new_message (enum SERVERS server, const char *room, const char *nick, const char *message) {
	char buf[255];
	char *s;

	switch (server) {
		case TWITCH_SERVER:
			snprintf (buf, 255, "test hello %s\n", nick);
			s = twitch_private_message (room, zl_config_get_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_NICK), buf);
			network_send (TWITCH_SERVER, s, strlen (s));
			free (s);
			break;
	}
}

const char *plugin_get_name () {
	return "test plugin";
}
