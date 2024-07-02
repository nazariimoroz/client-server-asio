project(client-server-asio)
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
find_package(Boost 1.71.0 COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(${PROJECT_NAME}
		main.cpp 
		server.h 
		server.cpp 
		client.h
		client.cpp)

	if (WIN32)
		target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ws2_32)
	endif (WIN32)
endif()


