#include <shared.h>
#include <stdlib.h>
#include <stdio.h>

char *twitch_private_message (const char *room, const char *nick, const char *message) {
	char *buf = calloc (2048, 1);
	snprintf (buf, 2048, ":%s!%s@%s.tmi.twitch.tv PRIVMSG %s :%s\n",
			nick,
			nick,
			nick,
			room,
			message
		 );

	return buf;
} 
