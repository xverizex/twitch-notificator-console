#include <stdio.h>
#include <stdint.h>
#include <zlc_config/config.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <AL/al.h>
#include <AL/alc.h>

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

#define TWITCH_IRC_SITE          "irc.chat.twitch.tv"
#define MAX_BUFFER               2048

static struct zl_config *cfg;

static const char ping[] = "PING :tmi.twitch.tv";
static const char pong[] = "PONG :tmi.twitch.tv";
static const char default_line[] = ":tmi.twitch.tv";
static const char join[] = "JOIN";
static const char msg[] = "PRIVMSG";

static uint16_t twitch_port = 6667;
static ALCdevice *device;
static ALCcontext *ctx;
static uint32_t source;

static void parse_buffer_for_message (char *room, char *nick, char *message, char **s) {
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
		printf ("\033[1;32m[%s]\033[00m\033[1;36m[%s]\033[00m: %s\n",
				room,
				nick,
				message
		       );
	}

	count = 0;
	if (!strncmp (m, join, sizeof (join) - 1)) {
		m += sizeof (join);
		while (*m != ' ' && *m != '\n') {
			room[count++] = *m++;
		}
		room[count] = 0;
		printf ("\033[1;36m%s\033[00m joined to room \033[1;32m%s\033[00m\n",
				nick,
				room
		       );

		alSourcePlay (source);
	}
}

int main (int argc, char **argv) {

	/* init config */
	cfg = zl_config_init (N_GROUPS);
	zl_config_init_group (cfg, GROUP_TWITCH, "Twitch", N_TWITCH_OPTS);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_TMI, ZL_TYPE_STRING, "tmi", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_CHANNEL, ZL_TYPE_STRING, "channel", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_NICK, ZL_TYPE_STRING, "nick", NULL);
	zl_config_add_option (cfg, GROUP_TWITCH, OPT_TWITCH_ALERT, ZL_TYPE_STRING, "alert", NULL);

	zl_config_parse (cfg, "twitch.conf");

	device = alcOpenDevice (NULL);
	ctx = alcCreateContext (device, NULL);
	alcMakeContextCurrent (ctx);

	uint32_t buf;
	alGenBuffers (1, &buf);

	/* read file */
	FILE *fp = fopen (zl_config_get_string (cfg, GROUP_TWITCH, OPT_TWITCH_ALERT), "r");
	fseek (fp, 0, SEEK_END);
	size_t len_file = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	char *data = malloc (len_file);
	fread (data, 1, len_file, fp);
	fclose (fp);

	alBufferData (buf, AL_FORMAT_MONO16, data, len_file, 44100);

	alGenSources (1, &source);
	alSourcei (source, AL_BUFFER, buf);
	alSourcePlay (source);

	struct hostent *ht = gethostbyname (TWITCH_IRC_SITE);

	struct sockaddr_in s;
	memset (&s, 0, sizeof (s));

	s.sin_family = AF_INET;
	s.sin_port = htons (twitch_port);
	s.sin_addr.s_addr = *((in_addr_t *) ht->h_addr);

	int sock = socket (AF_INET, SOCK_STREAM, 0);
	int ret = connect (sock, (const struct sockaddr *) &s, sizeof (s));
	if (ret == -1) {
		perror ("connect to twitch");
		exit (0);
	}

	char buffer[MAX_BUFFER];
	memset (buffer, 0, MAX_BUFFER);
	
	int len_write = snprintf (buffer, MAX_BUFFER, "PASS %s\n", zl_config_get_string (cfg, GROUP_TWITCH, OPT_TWITCH_TMI));
	ret = write (sock, buffer, len_write);
	if (ret == -1) {
		perror ("write 0;");
		exit (0);
	}
	len_write = snprintf (buffer, MAX_BUFFER, "NICK %s\n", zl_config_get_string (cfg, GROUP_TWITCH, OPT_TWITCH_NICK));
	ret = write (sock, buffer, len_write);
	if (ret == -1) {
		perror ("write 1;");
		exit (0);
	}
	len_write = snprintf (buffer, MAX_BUFFER, "JOIN #%s\n", zl_config_get_string (cfg, GROUP_TWITCH, OPT_TWITCH_CHANNEL));
	ret = write (sock, buffer, len_write);
	if (ret == -1) {
		perror ("write 2;");
		exit (0);
	}

	char room[512];
	char nick[64];
	char message[MAX_BUFFER];
	while (1) {
		int read_bytes = read (sock, buffer, MAX_BUFFER - 1);
		if (read_bytes < 0) break;

		buffer[read_bytes] = 0;
		char *s = buffer;

		if (!strncmp (buffer, default_line, sizeof (default_line))) continue;

		if (!strncmp (buffer, ping, sizeof (ping))) {
			write (sock, pong, sizeof (pong));
			continue;
		}

		parse_buffer_for_message (room, nick, message, &s);
	}
}
