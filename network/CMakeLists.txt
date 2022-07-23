cmake_minimum_required (VERSION 3.22)

project (network)

add_library (network SHARED network.c)

target_include_directories (network PUBLIC
	".."
	)

