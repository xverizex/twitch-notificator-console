#include <dlfcn.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <zlc_config/config.h>
#include "shared.h"

struct plugin_manager {
	int server;
	void *handle;
	void (*plugin_init) (int *server, struct zl_config *cfg);
	void (*plugin_receive_new_join) (enum SERVERS server, const char *nick, const char *room);
	void (*plugin_receive_new_message) (enum SERVERS server, const char *room, const char *nick, const char *message);
	const char *(*plugin_get_name) ();
};

static struct plugin_manager *pm;
static int size_pm;
extern struct zl_config *main_cfg;

void manager_plugin_init (const char **plugins, int size) {
	size_pm = size;

	pm = calloc (size_pm, sizeof (struct plugin_manager));

	for (int i = 0; i < size_pm; i++) {
		pm[i].handle = dlopen (plugins[i], RTLD_LAZY);
		pm[i].plugin_init = (void (*) (int *, struct zl_config *)) dlsym (pm[i].handle, "plugin_init");
		pm[i].plugin_receive_new_join = (void (*) (enum SERVERS, const char *, const char *)) dlsym (pm[i].handle, "plugin_receive_new_join");
		pm[i].plugin_receive_new_message = (void (*) (enum SERVERS, const char *, const char *, const char *)) dlsym (pm[i].handle, "plugin_receive_new_message");
		pm[i].plugin_get_name = (const char *(*) (void)) dlsym (pm[i].handle, "plugin_get_name");

		int server = N_SERVERS;
		pm[i].plugin_init (&server, main_cfg);
		pm[i].server = server;
	}
}

void manager_plugin_receive_new_join (enum SERVERS server, const char *nick, const char *room) {

	for (int i = 0; i < size_pm; i++) {
		if (pm[i].server == server || pm[i].server == N_SERVERS) {
			pm[i].plugin_receive_new_join (server, nick, room);
		}
	}
}

void manager_plugin_receive_new_message (enum SERVERS server, const char *room, const char *nick, const char *message) {

	for (int i = 0; i < size_pm; i++) {
		if (pm[i].server == server || pm[i].server == N_SERVERS) {
			pm[i].plugin_receive_new_message (server, room, nick, message);
		}
	}
}
