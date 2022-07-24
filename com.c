#include <stdio.h>
#include <string.h>
#include <zlc_config/config.h>
#include "config.h"
#include "network.h"
#include "main.h"
#include "parser.h"
#include "shared.h"


extern struct zl_config *main_cfg;

int com_twitch_init () {
	char buffer[MAX_BUFFER];
	memset (buffer, 0, MAX_BUFFER);

	int len_write;

	len_write = snprintf (buffer, MAX_BUFFER, "PASS %s\n", zl_config_get_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_TMI));
	network_send (TWITCH_SERVER, buffer, len_write);

	len_write = snprintf (buffer, MAX_BUFFER, "NICK %s\n", zl_config_get_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_NICK));
	network_send (TWITCH_SERVER, buffer, len_write);

	int size_array_channel;
	int ret = 0;
	const char **twitch_channels = zl_config_get_array_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_CHANNEL, &size_array_channel);
	for (int i = 0; i < size_array_channel; i++) {
		len_write = snprintf (buffer, MAX_BUFFER, "JOIN #%s\n", twitch_channels[i]);
		network_send (TWITCH_SERVER, buffer, len_write);
		ret = network_read (TWITCH_SERVER, buffer, MAX_BUFFER - 1);
		if (ret == -1) break;
		parser_twitch_rule (buffer);
	}

	return ret;
}
