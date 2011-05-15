#include "socket.h"

// Initialize WSA 
namespace wsa
{
	struct WSA_init{
		WSA_init();
		~WSA_init();
	};
}

wsa::WSA_init::WSA_init() 
{
	WSADATA wsaData;
	int res = WSAStartup( MAKEWORD(2,2), &wsaData );
	switch(res)
	{
	case NO_ERROR: 
		break;
	case WSASYSNOTREADY:
		error("WSAStartup error: The underlying network subsystem is not ready for network communication");
	case WSAVERNOTSUPPORTED:
		error("WSAStartup error: Windows Sockets 2.2 is not supported by this Windows Sockets implementation");
	case WSAEINPROGRESS:
		error("WSAStartup error: A blocking Windows Sockets 1.1 operation is in progress.");
	case WSAEPROCLIM:
		error("WSAStartup error: Limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
	case WSAEFAULT:
		error("WSAStartup error: WSADATA is not valid.");
	default:
		error("WSAStartup error: Unexpected error %d.", res);
	}
}
wsa::WSA_init::~WSA_init() 
{
	WSACleanup();
}

void wsa::WSAInitialize()
{
	static wsa::WSA_init instance;
}

SOCKET wsa::socket::detachHandle() 
{
	SOCKET res = m_socket;
	m_socket = INVALID_SOCKET;
	return res; 
}

wsa::socket& wsa::socket::operator=(SOCKET s) 
{
	if (s != getHandle())
	{
		close(); 
		m_socket = s; 
	}
	return * this;
}

wsa::socket& wsa::socket::operator=(wsa::socket&& s) 
{
	if (s.getHandle() != getHandle())
	{
		close(); 
		m_socket = s.detachHandle(); 
	}
	return * this;
}

void wsa::socket::close() 
{
	if (is_valid())
	{
		closesocket(m_socket); 
		m_socket = INVALID_SOCKET;
	}
}

wsa::socket wsa::connect(const char* server, int port)
{
	socket s = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( !s.is_valid() ) 
		WinAPIError("Error at socket()");		

	//Connect to the server. 
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( server );
	addr.sin_port = htons( port );

	int res = ::connect( s.getHandle(), (SOCKADDR*) &addr, sizeof(addr) );
	if (res == SOCKET_ERROR)
		WinAPIError("Error at connect() to %s:%d", server, port);		

	return s;
}

int wsa::socket::recv(char* buf, int len)
{
	int res = ::recv(getHandle(), buf, len, 0);
	if (res == SOCKET_ERROR)
		WinAPIError("Error at recv()");
	return res;
}

void wsa::socket::send(const char* buf, int len)
{
	int res = ::send(getHandle(), buf, len, 0);
	if (res == SOCKET_ERROR)
		WinAPIError("Error at send()");
	if (res != len)
		error("Error sending message, only %d of %d bytes was sent", res, len);
}

wsa::socket& wsa::socket::operator<<(const char* s)
{
	send(s, strlen(s));
	return *this;
}

wsa::socket& wsa::socket::operator>>(std::string& s)
{
	char buf[1024 * 4];
	int len = recv(buf, sizeof(buf) - 1);
	s.assign(buf, len);
	return *this;
}

wsa::server_socket wsa::listen(const char* server, int port, int backlog)
{
	server_socket s = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( !s.is_valid() ) 
		WinAPIError("socket() failed");		

	//Bind the socket. 
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( server );
	service.sin_port = htons( port );

	if ( ::bind( s.getHandle(), (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) 
		WinAPIError( "bind() failed for %s:%d", server, port );

	//Listen on the socket. 
	if ( ::listen( s.getHandle(), backlog ) == SOCKET_ERROR )
		WinAPIError("Error listening socket bind on %s:%d", server, port );

	return s;
}

wsa::socket wsa::server_socket::accept()
{
	return ::accept( getHandle(), NULL, NULL );
}
