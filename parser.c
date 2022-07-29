#include <stdio.h>
#include <string.h>
#include "main.h"
#include "sound.h"
#include "network.h"
#include "shared.h"
#include "history.h"
#include <pthread.h>
#include "lua.h"

static const char ping[] = "PING :tmi.twitch.tv";
static const char pong[] = "PONG :tmi.twitch.tv\r\n";
static const char default_line[] = ":tmi.twitch.tv";
static const char join[] = "JOIN";
static const char msg[] = "PRIVMSG";

extern pthread_mutex_t mutex;

extern void manager_plugin_receive_new_join (enum SERVERS server, const char *nick, const char *room);
extern void manager_plugin_receive_new_message (enum SERVERS server, const char *room, const char *nick, const char *message);

static void parser_twitch_buffer_for_message (char *room, char *nick, char *message, char **s) {
	char *m = *s;
	m++;
	int count = 0;
	while (*m != '!' && *m != '\n') {
		nick[count++] = *m++;
	}
	nick[count] = 0;
	if (*m == '\n') return;
	while (*m != ' ' && *m != '\n') m++;
	if (*m == '\n') return;
	m++;
	count = 0;
	if (!strncmp (m, msg, sizeof (msg) - 1)) {
		m += sizeof (msg);
		while (*m != ' ' && *m != '\n') {
			room[count++] = *m++;
		}
		room[count] = 0;
		m += 2;

		count = 0;
		while (*m != '\n') {
			message[count++] = *m++;
		}
		message[count] = 0;
		pthread_mutex_lock (&mutex);
		printf ("\033[1;33m[TWITCH]\033[00m\033[1;32m[%s]\033[00m\033[1;36m[%s]\033[00m: %s\n",
				room,
				nick,
				message
		       );

		sound_play ();
		history_add (TWITCH_SERVER, room, nick, message);
		manager_plugin_receive_new_message (TWITCH_SERVER, room, nick, message);
		manager_lua_receive_new_message (TWITCH_SERVER, room, nick, message);
		pthread_mutex_unlock (&mutex);
	}

	count = 0;
	if (!strncmp (m, join, sizeof (join) - 1)) {
		m += sizeof (join);
		while (*m != ' ' && *m != '\n') {
			room[count++] = *m++;
		}
		room[count] = 0;
		pthread_mutex_lock (&mutex);
		printf ("\033[1;33m[TWITCH]\033[00m\033[1;36m%s\033[00m joined to room \033[1;32m%s\033[00m\n",
				nick,
				room
		       );
		sound_play ();
		manager_plugin_receive_new_join (TWITCH_SERVER, nick, room);
		pthread_mutex_unlock (&mutex);
	}
}

void parser_twitch_rule (char *buffer) {
	char *s = buffer;

	char room[512];
	char nick[64];
	char message[MAX_BUFFER];

	if (!strncmp (buffer, default_line, sizeof (default_line))) return;

	if (!strncmp (buffer, ping, sizeof (ping))) {
		network_send (TWITCH_SERVER, pong, sizeof (pong));
		return;
	}

	parser_twitch_buffer_for_message (room, nick, message, &s);
}
