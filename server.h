#pragma once
#include <iostream>
#include <fstream>
#include <mutex>

#include <boost/asio.hpp>

class Server
{
	const std::string DELIM = "\1\2\3\4";
	using tcp = boost::asio::ip::tcp;
public:
	explicit Server( const std::string& address, unsigned short port );
	Server( const Server& ) = delete;
	Server( Server&& ) = delete;

	~Server();

	void start();

private:
	void clients_thread( tcp::socket&& socket );
	std::string read_from_client( tcp::socket& socket );
	void read_file_from_client( tcp::socket& socket, std::string& out_file_name );
	void write_to_client( tcp::socket& socket, const std::string& message );
	void mutex_cout( const std::string& message );

private:
	boost::asio::io_service m_service;
	tcp::acceptor m_tcp_acceptor;
	std::mutex m_cout_mtx;
};
