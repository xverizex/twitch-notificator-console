cmake_minimum_required (VERSION 3.22)

project (twitch_terminal)

include (FindPkgConfig)

add_executable (twitch_terminal main.c com.c config.c network.c parser.c sound.c plugin.c)

add_subdirectory (zlc_config)

pkg_check_modules (AL "openal")

target_include_directories (twitch_terminal PUBLIC
	"."
	${AL_INCLUDE_DIRS}
	)

target_link_libraries (twitch_terminal PUBLIC
	zlc_config
	${AL_LIBRARIES}
	)

file (COPY "twitch.conf" DESTINATION ${CMAKE_BINARY_DIR})
