#include "config.h"
#include <zlc_config/config.h>
#include <stdio.h>

struct zl_config *config_init () {

	struct zl_config *cfg = zl_config_init (N_GROUPS);
	zl_config_init_group (cfg, GROUP_TWITCH, "Twitch", N_TWITCH_OPTS);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_TMI, ZL_TYPE_STRING, "tmi", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_CHANNEL, ZL_TYPE_ARRAY_STRING, "channel", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_NICK, ZL_TYPE_STRING, "nick", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_ALERT, ZL_TYPE_STRING, "alert", NULL);

	zl_config_init_group (cfg, GROUP_SHARED, "Shared", N_SHARED_OPTS);
	zl_config_add_option (cfg, GROUP_SHARED, OPT_SHARED_PLUGINS, ZL_TYPE_ARRAY_STRING, "plugins", NULL);

	zl_config_parse (cfg, "twitch.conf");
	
	return cfg;
}
