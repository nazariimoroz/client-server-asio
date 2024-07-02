#pragma once
#include <iostream>
#include <fstream>

#include <boost/asio.hpp>	


class Client
{
	const std::string DELIM = "\1\2\3\4";
	using tcp = boost::asio::ip::tcp;
public:
	Client( const std::string& address, unsigned short port );

	void start();

private:
	std::string read_from_server();
	void write_to_server( const std::string& message );
	void write_file_to_server( const std::string& file_path );

private:
	boost::asio::io_service m_service;
	tcp::resolver m_resolver;
	tcp::socket m_socket;
};

