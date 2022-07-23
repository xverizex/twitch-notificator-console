cmake_minimum_required (VERSION 3.22)

project (test)

add_library (test SHARED main.c)

target_include_directories (test PUBLIC
	"../.."
	)

target_link_libraries (test PUBLIC
	"-Wl,-rpath network -L../../network -lnetwork -L../../answers -lanswers"
	"-L../../zlc_config -lzlc_config"
	"-Wl,-rpath answers -L../../answers -lanswers"
	)
