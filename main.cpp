#include <iostream>
#include <string>

#include "server.h"
#include "client.h"

using namespace std::string_literals;


int main(int argc, char* argv[])
{
	if(argc < 3 || argc > 4)
	{
		std::cout << "Usage: " << argv[0] << " <client|server> < <localhost> | <online_server> | <address> <port>>" << std::endl;
		return 1;
	}
	std::string address;
	std::uint16_t port;
	if( argv[2] == "online_server"s )
	{
		address = "79.132.138.89";
		port = 7134;
	}
	else if( argv[2] == "localhost"s )
	{
		address = "127.0.0.1";
		port = 8080;
	}
	else if( argc == 4 )
	{
		address = argv[2];
		port = std::stoul( argv[3] );
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " <client|server> < <localhost> | <online_server> | <address> <port>>" << std::endl;
		return 1;
	}

	try
	{
		if( argv[1] == "client"s )
		{
			Client client( address, port );
			client.start();
		}
		else if( argv[1] == "server"s )
		{
			Server server( address, port );
			server.start();
		}
		else
		{
			std::cout << "Usage: " << argv[0] << " <client|server> < <localhost> | <online_server> | <address> <port>>" << std::endl;
			return 1;
		}
	} catch( const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
