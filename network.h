#pragma once

#include <stdint.h>
#include <stdlib.h>

int network_init (int index, const char *url, uint16_t port);
void network_send (int index, const char *buffer, size_t len);
int network_read (int index, char *buffer, size_t max_buffer);
void network_close_connection (int index);
