enum Groups {
	GROUP_TWITCH,
	N_GROUPS
};

enum OptionsTwitch {
	OPT_TWITCH_TMI,
	OPT_TWITCH_CHANNEL,
	OPT_TWITCH_NICK,
	OPT_TWITCH_ALERT,
	N_TWITCH_OPTS
};

struct zl_config *config_init ();
