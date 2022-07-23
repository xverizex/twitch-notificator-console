enum Groups {
	GROUP_TWITCH,
	GROUP_GENERAL,
	N_GROUPS
};

enum OptionsTwitch {
	OPT_TWITCH_TMI,
	OPT_TWITCH_CHANNEL,
	OPT_TWITCH_NICK,
	OPT_TWITCH_ALERT,
	OPT_TWITCH_PLUGINS,
	N_TWITCH_OPTS
};

enum OptionsGeneral {
	OPT_GENERAL_PLUGINS,
	N_GENERAL_OPTS
};

struct zl_config *config_init ();
