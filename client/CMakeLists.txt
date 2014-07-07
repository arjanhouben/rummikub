project( rummikub_client )

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -std=c++11\ -Wall )

add_library( r_client SHARED
	main.cpp
)
