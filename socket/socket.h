#pragma once
#include <exception>
#include <string>
#include <winsock2.h>
#include "error/error.h"

// auto link
#ifdef _DEBUG
	#pragma comment(lib, "socketd.lib")
#else
	#pragma comment(lib, "socket.lib")
#endif
#pragma comment(lib, "Ws2_32.lib")

namespace wsa
{
	// Initialize WSA 
	void WSAInitialize();

	class socket{
		SOCKET m_socket;
		socket(const socket&); // disabled copy constructor 
		void operator=(const socket&); // disabled copy operator
	public:
		socket() : m_socket(INVALID_SOCKET) {}
		socket(SOCKET s) : m_socket(s) {}
		socket(socket&& s) : m_socket(s.detachHandle()){} // move constructor 
		~socket() {close();}

		void close();
		bool is_valid() const {return m_socket != INVALID_SOCKET;}
		SOCKET getHandle() const {return m_socket;}
		SOCKET detachHandle();
		socket& operator=(SOCKET s);
		socket& operator=(socket&& s);

		int recv(char* buf, int len);
		void send(const char* buf, int len);

		socket& operator<<(const char* s);
		socket& operator>>(std::string& s);
	};

	class server_socket : public wsa::socket
	{
		server_socket(const server_socket&); // disabled copy constructor 
		void operator=(const server_socket&); // disabled copy operator
	public:
		server_socket(){}
		server_socket(SOCKET s) 
			: wsa::socket(s) {}
		server_socket(server_socket&& s) 
			: wsa::socket(std::move(s)) {} // move constructor 

		void start_game();
		void stop_game();

		socket accept();
	};

	socket connect(const char* server, int port);
	inline socket connect(const std::string& server, int port) {return connect(server.c_str(), port);}

	server_socket listen(const char* server, int port, int backlog = SOMAXCONN);
	inline server_socket listen(const std::string& server, int port, int backlog = SOMAXCONN) {return listen(server.c_str(), port, backlog);}
}