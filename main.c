#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlc_config/config.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include <pthread.h>
#include "sound.h"
#include "network.h"
#include "main.h"
#include "parser.h"
#include "com.h"
#include "shared.h"
#include "history.h"
#include "lua.h"

struct zl_config *main_cfg;

static pthread_t *ts;

pthread_mutex_t mutex;

extern void manager_plugin_init (const char **plugins, int size);

struct server {
	int rule;
	const char *url;
	uint16_t port;
	void (*parse_func) (char *data);
	int index;
} server[] = {
	{ TWITCH_SERVER, TWITCH_IRC_SITE, 6667, parser_twitch_rule }
};

void *thread_channel (void *data_from_main_thread) {

	int index = ((int) data_from_main_thread);

	sound_init (zl_config_get_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_ALERT));

	char buffer[MAX_BUFFER];

	int ret;

	while (1) {
		if ((ret = network_init (index, server[index].url, server[index].port)) == -1) {sleep (1); continue;}
		if ((ret = com_twitch_init ()) == -1) {network_close_connection (index); sleep (1); continue;}

		while (1) {
			ret = network_read (index, buffer, MAX_BUFFER - 1);
			if (ret <= 0) break;
			server[index].parse_func (buffer);
	
		}

		network_close_connection (index);
		printf ("Connection is close on %d server\n", index);
		sleep (1);
	}

	return NULL;
}


int main (int argc, char **argv) {

	/* init config */
	main_cfg = config_init ();

	history_init ();

	pthread_mutex_init (&mutex, NULL);

	int size_plugins;
	const char **plugins = zl_config_get_array_string (main_cfg, GROUP_GENERAL, OPT_GENERAL_PLUGINS, &size_plugins);
	manager_plugin_init (plugins, size_plugins);
	manager_lua_init ();

	ts = calloc (N_SERVERS, sizeof (pthread_t));

	for (int i = 0; i < N_SERVERS; i++) {
		pthread_create (&ts[i], NULL, thread_channel, (void *) i);
	}

	for (int i = 0; i < N_SERVERS; i++) {
		int *val;
		pthread_join (ts[i], (void **) &val);
	}
}
