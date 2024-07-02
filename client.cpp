#include "client.h"
#include <filesystem>
#include <stdexcept>

namespace bst
{
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
};

Client::Client( const std::string& address, unsigned short port )
	: m_resolver( m_service )
	, m_socket( m_service )
{
	tcp::resolver::results_type endpoint
		= m_resolver.resolve( tcp::endpoint( bst::address::from_string( address ), port ));
	bst::connect( m_socket, endpoint );
}

void Client::start()
{
	while( true )
	{
		std::cout << "Full path to file: ";
		std::string file_path;
		std::getline( std::cin, file_path );

		try
		{
			write_file_to_server( file_path );
		} catch( const std::ios_base::failure& e )
		{
			std::cout << e.what() << std::endl;
			continue;
		} catch( const std::exception& e )
		{
			std::cout << e.what() << std::endl;
			return;
		}

		std::cout << "Server: " << read_from_server() << std::endl;
	}
}

std::string Client::read_from_server()
{
	bst::streambuf buffer;
	bst::read_until( m_socket, buffer, DELIM );
	std::string message = bst::buffer_cast<const char*>( buffer.data() );
	message.erase( message.end() - DELIM.size(), message.end() );
	return message;
}

void Client::write_to_server( const std::string& message )
{
	bst::write( m_socket, bst::buffer( message + DELIM ) );
}

void Client::write_file_to_server( const std::string& file_path )
{
	const auto BUF_SIZE = 128;

	namespace fs = std::filesystem;

	std::ifstream ifile( file_path, std::ios::binary );
	if( !ifile.is_open() )
		throw std::ios_base::failure( "File have not found" );
	const auto file_name = fs::path( file_path ).filename().string();
	const auto file_size = fs::file_size( fs::path( file_path ) );

	write_to_server( file_name + "|" + std::to_string( file_size ));
	std::cout << "Server: " << read_from_server() << std::endl;
	char buffer[BUF_SIZE];
	while( !ifile.eof() && file_size )
	{
		ifile.read( buffer, BUF_SIZE );
		bst::write( m_socket, bst::buffer( buffer, ifile.gcount() ));
		std::cout << "Server: " << read_from_server() << std::endl;
	}

	write_to_server( "loading completed" );
	ifile.close();
}
