#include "TcpComm.h"

#include "Tools/Logger/Logger.h"

#include <iostream>
#ifndef WINCE
#include <cerrno>
#include <fcntl.h>
#endif

#ifdef WINDOWS

#ifndef WINCE
#include <sys/types.h>
#endif

#define ERRNO WSAGetLastError()
#define RESET_ERRNO WSASetLastError(0)
#undef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#define NON_BLOCK(socket) ioctlsocket(socket, FIONBIO, (u_long*) "NONE")
#define CLOSE(socket) closesocket(socket)

class _WSAFramework
{
public:
	_WSAFramework()
	{
		WORD wVersionRequested = MAKEWORD(1, 0);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);
	}
	~_WSAFramework() { WSACleanup(); }
} _wsaFramework;

#else

#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define ERRNO errno
#define RESET_ERRNO errno = 0
#define NON_BLOCK(socket) fcntl(socket,F_SETFL,O_NONBLOCK)
#define CLOSE(socket) close(socket)

#endif

SocketServerTCP::SocketServerTCP(int port) : SocketTCP()
{
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
	m_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket > 0)
	{
		int val = 1;
		setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
		m_address.sin_addr.s_addr = INADDR_ANY;
		if (bindSocket())
			LOGGER_ERROR("Response Packet Client", "Problem Binding");
		if (listenSocket())
			LOGGER_ERROR("Response Packet Client", "Problem Listening");
		if (m_isNonBlock)
			NON_BLOCK(m_socket); // switch socket to nonblocking
	}
}

SocketServerTCP::~SocketServerTCP()
{
	closeSocket();
}

int SocketServerTCP::acceptClient()
{
#ifndef WINDOWS
	unsigned int addrlen = sizeof(sockaddr_in);
#else
	int addrlen = sizeof(sockaddr_in);
#endif

	return (int)accept(m_socket, (sockaddr*)&m_address, &addrlen);
}

bool SocketServerTCP::bindSocket()
{
	return bind(m_socket, (sockaddr*)&m_address, sizeof(sockaddr_in)) < 0;
}

bool SocketServerTCP::listenSocket()
{
	return listen(m_socket, SOMAXCONN) < 0;
}

SocketClientTcp::SocketClientTcp(const char* ip, int port) :
	m_bWasConnected(false), SocketTCP()
{
	m_isNonBlock = true;
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
	if (ip)
	{
		struct hostent* he;
		if ((he = gethostbyname(ip)) == NULL)
			m_address.sin_addr.s_addr = inet_addr(ip);
		else
			memcpy((void *)&m_address.sin_addr, (const void *)he->h_addr_list[0], he->h_length);
	}

	checkConnection();
}

SocketClientTcp::SocketClientTcp(int socketClient) :
	m_bWasConnected(false)
{
	m_socket = socketClient;

	if (connected())
	{
#ifndef WINDOWS
		unsigned int addrlen = sizeof(sockaddr_in);
#else
		int addrlen = sizeof(sockaddr_in);
#endif
		getpeername(m_socket, (sockaddr*)&m_address, &addrlen);

		if (m_isNonBlock)
			NON_BLOCK(m_socket); // switch socket to nonblocking
	}

	checkConnection();
}

SocketClientTcp::~SocketClientTcp()
{
	if (connected())
		closeSocket();
}

bool SocketClientTcp::checkConnection()
{
	if (!connected())
	{
		if (!m_bWasConnected)
		{
			m_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
			if (connect(m_socket, (sockaddr*)&m_address, sizeof(sockaddr_in)) != 0)
				closeSocket();
		}

		if (connected())
		{
			m_bWasConnected = true;

			if (m_isNonBlock)
				NON_BLOCK(m_socket); // switch socket to nonblocking
#ifdef MACOSX
			int yes = 1;
			if (!setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes)))
				return false;
#endif
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

bool SocketClientTcp::receive(char* buffer, int size, bool wait)
{
	if (!checkConnection())
		return false;

	if (!wait)
	{
		RESET_ERRNO;
#ifndef WINDOWS
		int received = recv(m_socket, buffer, size, MSG_PEEK);
		if (received < size)
		{
			if (!received || (received < 0 && ERRNO != EWOULDBLOCK && ERRNO != EINPROGRESS))
				closeSocket();
			return false;
		}
#else
		char c;
		int received = recv(m_socket, &c, 1, MSG_PEEK);
		if (!received || (received < 0 && ERRNO != EWOULDBLOCK && ERRNO != EINPROGRESS) ||
			ioctlsocket(m_socket, FIONREAD, (u_long*)&received) != 0)
		{
			closeSocket();
			return false;
		}
		else if (received == 0)
			return false;
#endif
	}

	int received = 0;
	while (received < size)
	{
		RESET_ERRNO;

		int received2 = recv(m_socket, (char*)buffer + received,
			size - received, 0);

		if (!received2 || (received2 < 0 && ERRNO != EWOULDBLOCK && ERRNO != EINPROGRESS))  // error during reading of package
		{
			closeSocket();
			return false;
		}
		else if (ERRNO == EWOULDBLOCK || ERRNO == EINPROGRESS) // wait for the rest
		{
			received2 = 0;
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;
			fd_set rset;
			FD_ZERO(&rset);
			FD_SET(m_socket, &rset);
			if (select(m_socket + 1, &rset, 0, 0, &timeout) == -1)
			{
				closeSocket();
				return false; // error while waiting
			}
		}
		received += received2;
	}
	return true; // ok, data received
}

int SocketClientTcp::receiveSys(void* buffer, unsigned int len, int flags)
{
	return recv(m_socket, (char*)buffer, len, flags);
}

bool SocketClientTcp::send(const char* buffer, int size)
{
	if (!checkConnection())
		return false;

	RESET_ERRNO;
	int sent = ::send(m_socket, (const char*)buffer, size, 0);
	if (sent > 0)
	{
		while (sent < size && (ERRNO == EWOULDBLOCK || ERRNO == EINPROGRESS || ERRNO == 0))
		{
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;
			fd_set wset;
			FD_ZERO(&wset);
			FD_SET(m_socket, &wset);
			RESET_ERRNO;
			if (select(m_socket + 1, 0, &wset, 0, &timeout) == -1)
				break;
			RESET_ERRNO;
			int sent2 = ::send(m_socket, (const char*)buffer + sent, size - sent, 0);
			if (sent2 >= 0)
			{
				sent += sent2;
			}
		}
	}

	if (ERRNO == 0 && sent == size)
		return true;
	else
	{
		closeSocket();
		return false;
	}
}
