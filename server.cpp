#include "server.h"
#include <thread>

namespace bst
{
	using namespace boost;
	using namespace boost::asio;
	using namespace boost::asio::ip;
	using namespace boost::system;
};
using namespace std::string_literals;


Server::Server( const std::string& address, unsigned short port )
	: m_tcp_acceptor( m_service, tcp::endpoint( bst::address::from_string(address), port))
{
}

Server::~Server()
{
	try
	{
		m_service.stop();
		m_tcp_acceptor.close();
	} catch( const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void Server::start()
{
	while( true )
	{
		tcp::socket socket( m_service );
		m_tcp_acceptor.accept( socket );

		std::thread thr( &Server::clients_thread, this, std::move( socket ) );
		thr.detach();
	}
}

void Server::clients_thread( tcp::socket&& socket )
{
	mutex_cout( "Client accepted" );
	std::string file_name;
	try
	{
		while( socket.is_open() )
		{
			read_file_from_client( socket, file_name );

			mutex_cout( "Client: " + read_from_client( socket ) );
			write_to_client( socket, file_name + " have created successful" );
		}
	} 
	catch( const std::ios_base::failure& e )
	{
		mutex_cout( e.what() );
		mutex_cout( "Client: " + read_from_client( socket ) );
		write_to_client( socket, file_name + " have not created successful" );
	}catch( const std::exception& e )
	{
		mutex_cout( e.what() );
	}
	mutex_cout( "Client disconnected" );
	socket.close();
}

std::string Server::read_from_client( tcp::socket& socket )
{
	bst::streambuf file_name_buf;
	bst::read_until( socket, file_name_buf, DELIM );
	std::string file_name = bst::buffer_cast<const char*>( file_name_buf.data() );
	file_name.erase( file_name.end() - DELIM.size(), file_name.end());
	return file_name;
}

void Server::read_file_from_client( tcp::socket& socket, std::string& out_file_name )
{
	const auto BUF_SIZE = 128;
	const auto inf = read_from_client( socket );
	write_to_client( socket, "file name accepted" );
	const auto file_name = inf.substr(0, inf.find('|'));
	const auto file_size = stoull( inf.substr( inf.find( '|' ) + 1 ) );

	std::ofstream ofile(file_name, std::ios::binary);
	if( !ofile.is_open() )
		throw std::ios_base::failure( "File have not created" );

	const auto EXT = file_size % BUF_SIZE;
	for( std::uintmax_t i = 0; 
		 i < file_size;) 
	{
		i += ( (file_size - EXT) != i ) ? BUF_SIZE : EXT;
		const auto TO_READ  = ( i % BUF_SIZE == 0 ) ? BUF_SIZE : EXT;
		bst::streambuf buf;
		bst::read( socket, buf, bst::transfer_exactly( TO_READ ) );
		write_to_client( socket, "loading..." );
		ofile.write( bst::buffer_cast<const char*>( buf.data() ), TO_READ);
	}

	ofile.close();

	out_file_name =  file_name;
}

void Server::write_to_client( tcp::socket& socket, const std::string& message )
{
	bst::write( socket, bst::buffer( message + DELIM ) );
}

void Server::mutex_cout( const std::string& message )
{
	std::unique_lock _( m_cout_mtx );
	std::cout << message << std::endl;
}
