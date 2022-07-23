#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "shared.h"

static int server[N_SERVERS];

void network_init (int index, const char *site, uint16_t port) {
	struct hostent *ht = gethostbyname (site);

	struct sockaddr_in s;
	memset (&s, 0, sizeof (s));

	s.sin_family = AF_INET;
	s.sin_port = htons (port);
	s.sin_addr.s_addr = *((in_addr_t *) ht->h_addr);

	int sock = socket (AF_INET, SOCK_STREAM, 0);
	int ret = connect (sock, (const struct sockaddr *) &s, sizeof (s));
	if (ret == -1) {
		perror ("connect to twitch");
		exit (0);
	}

	server[index] = sock;
}

void network_send (int index, const char *buffer, size_t len) {
	size_t ret = write (server[index], buffer, len);
	if (ret == -1) {
		perror ("network_send");
		exit (0);
	}
}

int network_read (int index, char *buffer, size_t max_buffer) {
	size_t read_bytes = read (server[index], buffer, MAX_BUFFER - 1);
	if (read_bytes <= 0) return -1;

	buffer[read_bytes] = 0;
	return read_bytes;
}

void network_close_connection (int index) {
	close (server[index]);
}
