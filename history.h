#pragma once

void history_init ();
void history_add (enum SERVERS server, const char *room, const char *user, const char *message);
