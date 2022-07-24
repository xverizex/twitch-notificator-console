#pragma once

void manager_lua_init ();
void manager_lua_receive_new_message (int server, const char *room, const char *nick, const char *message);
