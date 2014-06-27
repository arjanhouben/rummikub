project( rummikub_server )

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -std=c++11\ -Wall )

add_executable( r_server
	src/main.cpp
)
