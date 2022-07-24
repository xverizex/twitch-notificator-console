#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlc_config/config.h>
#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <pthread.h>
#include "config.h"
#include "network.h"
#include "answers.h"
#include "shared.h"

static lua_State **l;
static int count_l;
extern struct zl_config *main_cfg;
extern pthread_mutex_t mutex;

static void load_all_scripts_from_directory (const char *dir_path);

void manager_lua_init () {

	int size = 0;
	const char **p = zl_config_get_array_string (main_cfg, GROUP_GENERAL, OPT_GENERAL_LUA_SCRIPTS, &size);

	for (int i = 0; i < size; i++) {
		load_all_scripts_from_directory (p[i]);
	}
}

static int net_send_message (lua_State *L) {
	int server = lua_tointeger (L, 1);
	const char *room = lua_tostring (L, 2);
	const char *message = lua_tostring (L, 3);

	char *s = NULL;
	char buf[4096];
	switch (server) {
		case TWITCH_SERVER:
			s = twitch_private_message (room, zl_config_get_string (main_cfg, GROUP_TWITCH, OPT_TWITCH_NICK), message);
			int ret = network_send (TWITCH_SERVER, s, strlen (s));
			break;
	}
	if (s) free (s);
}


static void load_all_scripts_from_directory (const char *dir_path) {

	if (access (dir_path, F_OK)) {
		fprintf (stderr, "dir lua scripts: %s;", dir_path);
		perror (" ");
		return;
	}

	DIR *dir = opendir (dir_path);

	struct dirent *d;
	char *found;

	char lua_path_file[255];

	while (d = readdir (dir)) {
		if (!(found = strstr (d->d_name, ".lua\0"))) continue;
		int index = count_l++;
		l = realloc (l, sizeof (void *) * count_l);
		l[index] = luaL_newstate ();

		luaL_openlibs (l[index]);
		lua_register (l[index], "net_send_message", net_send_message);

		snprintf (lua_path_file, 255, "%s/%s", dir_path, d->d_name);
		if (luaL_loadfilex (l[index], lua_path_file, NULL) != LUA_OK) {
			fprintf (stderr, "someting wrong with load lua script: %s; code error: %d\n", lua_path_file, lua_error (l[index]));
		}
		if (lua_pcall (l[index], 0, 0, 0) != LUA_OK) {
			fprintf (stderr, "someting wrong with run lua script: %s; code error: %d\n", lua_path_file, lua_error (l[index]));
		}
	}

	closedir (dir);
}

void manager_lua_receive_new_message (int server, const char *room, const char *nick, const char *message) {
	for (int i = 0; i < count_l; i++) {
		lua_getglobal (l[i], "receive_new_message");
		lua_pushinteger (l[i], server);
		lua_pushstring (l[i], room);
		lua_pushstring (l[i], nick);
		lua_pushstring (l[i], message);

		lua_call (l[i], 4, 0);
	}
}
