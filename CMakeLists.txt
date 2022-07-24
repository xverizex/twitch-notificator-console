cmake_minimum_required (VERSION 3.22)

project (twitch_terminal)

include (FindPkgConfig)

add_executable (twitch_terminal main.c com.c config.c parser.c sound.c plugin.c history.c lua.c)

add_subdirectory (zlc_config)
add_subdirectory (network)
add_subdirectory (answers)

add_subdirectory (plugins/test)

pkg_check_modules (AL "openal")
pkg_check_modules (SQLITE "sqlite3")
pkg_check_modules (LUA "lua53")

target_include_directories (twitch_terminal PUBLIC
	"."
	network
	answers
	${AL_INCLUDE_DIRS}
	${SQLITE_INCLUDE_DIRS}
	${LUA_INCLUDE_DIRS}
	)

target_link_libraries (twitch_terminal PUBLIC
	zlc_config
	network
	answers
	${AL_LIBRARIES}
	${SQLITE_LIBRARIES}
	${LUA_LIBRARIES}
	"-ldl"
	)

file (COPY "twitch.conf" DESTINATION ${CMAKE_BINARY_DIR})
file (COPY "plugins" DESTINATION ${CMAKE_BINARY_DIR})
file (COPY "lua_scripts" DESTINATION ${CMAKE_BINARY_DIR})
