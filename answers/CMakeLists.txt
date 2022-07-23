cmake_minimum_required (VERSION 3.22)

project (answers)

add_library (answers SHARED twitch.c)

target_include_directories (answers PUBLIC
	".."
	)
