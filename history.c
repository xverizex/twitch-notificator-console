#include <stdio.h>
#include <config.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shared.h"
#include <zlc_config/config.h>

extern struct zl_config *main_cfg;

static sqlite3 *s_db;
static int s_enabled;

void history_init () {
	s_enabled = zl_config_get_bool (main_cfg, GROUP_HISTORY, OPT_HISTORY_ENABLED);
	if (!s_enabled) return;

	const char *file_path = zl_config_get_string (main_cfg, GROUP_HISTORY, OPT_HISTORY_FILE_DB);
	if (file_path == NULL) {
		fprintf (stderr ,"## file_path is not set for db history\n");
		s_enabled = 0;
		return;
	}

	int ret = sqlite3_open (file_path, &s_db);
	if (ret != SQLITE_OK) {
		s_enabled = 0;
		fprintf (stderr, "%s\n", sqlite3_errmsg (s_db));
		sqlite3_close (s_db);
		return;
	}

	const char table[] = "CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY AUTOINCREMENT, server STRING NOT NULL, room STRING NOT NULL, user STRING NOT NULL, message STRING NOT NULL);";

	char *err_msg = NULL;

	ret = sqlite3_exec ( s_db,
			table,
			0,
			0,
			&err_msg
			);

	if (ret != SQLITE_OK) {
		fprintf (stderr, "create table history error: %s\n", err_msg);
		sqlite3_free (err_msg);
		sqlite3_close (s_db);
		s_enabled = 0;
		return;
	}
}

static void exec_query (const char *buf) {
	char *err_msg = 0;
	int ret = sqlite3_exec ( s_db,
			buf,
			0,
			0,
			&err_msg
			);

	if (ret != SQLITE_OK) {
		fprintf (stderr, "## query to save in history: %s\n", err_msg);
		sqlite3_free (err_msg);
	}
}


static void leave_quote (char **m, const char *u) {
	int len = strlen (u);
	int is = 0;
	int iu = 0;

	*m = calloc (len + 1, 1);
	char *s = *m;

	while (u[iu] != 0) {
		if (u[iu] == '\\') {
			iu++;
			continue;
		}
		if (u[iu] == '\'') {
			s[is++] = '\'';
			s[is++] = u[iu++];
			continue;
		}

		s[is++] = u[iu++];
	}
	s[is] = 0;
}

void history_add (enum SERVERS server, const char *room, const char *user, const char *message) {
	if (!s_enabled) return;

	char buf[4096];
	char *r = NULL;
	char *u = NULL;
	char *m = NULL;
	leave_quote (&r, room);
	leave_quote (&u, user);
	leave_quote (&m, message);

	switch (server) {
		case TWITCH_SERVER:
			snprintf (buf, 4096, "INSERT INTO history (server,room,user,message) VALUES ('TWITCH','%s','%s','%s');", r, u, m);
			exec_query (buf);
			break;
	}

	free (r);
	free (u);
	free (m);
}
