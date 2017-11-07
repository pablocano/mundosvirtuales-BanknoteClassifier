#pragma once

#include <string>

#ifdef WIN32

#ifdef NOMINMAX
#include <winsock.h>
#else
#define NOMINMAX
#include <winsock.h>
#undef NOMINMAX
#endif

#else
#include <netinet/in.h>
#endif

#ifdef WIN32
#define CLOSE(socket) closesocket(socket)
#else
#define CLOSE(socket) close(socket)
#endif

/// <summary>
/// Base class for socket TCP.
/// </summary>
class SocketTCP
{
protected:
	sockaddr_in m_address; /* The address connection. */
	int m_socket; /* The handle of the basic socket. */
	bool m_isNonBlock; /* If it is true the socket is non-blocking. */

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	SocketTCP() : m_socket(0), m_isNonBlock(false) {}

	/// <summary>
	/// Closes the transfer socket.
	/// </summary>
	void closeSocket()
	{
		if (m_socket > 0)
			CLOSE(m_socket);
		m_socket = 0;
	}

	/// <summary>
	/// Gets information about address connection.
	/// </summary>
	/// <returns>Returns string with information.</returns>
	std::string getInfo()
	{
		return std::string(inet_ntoa(m_address.sin_addr));
	}

	/// <summary>
	/// Gets handle socket.
	/// </summary>
	/// <returns>Returns handle socket.</returns>
	int getSocket() { return m_socket; }

	/// <summary>
	/// The function return whether the connection was successful.
	/// </summary>
	/// <returns>Returns true if the connection was established, false otherwise.</returns>
	bool connected() const { return m_socket > 0; }
};

/// <summary>
/// Class that allows to handle sockets for server deployments.
/// </summary>
class SocketServerTCP : public SocketTCP
{
public:
	/// <summary>
	/// Constructor.
	/// Opens a TCP connection to a remote host.
	/// </summary>
	/// <param name="port">Port The port over which will be communicated.</param>
	SocketServerTCP(int port);

	/// <summary>
	/// Destructor.
	/// </summary>
	~SocketServerTCP();

	/// <summary>
	/// Accept Clients.
	/// </summary>
	/// <returns>Returns socket client.</returns>
	int acceptClient();

protected:

	/// <summary>
	/// Binds the socket to an m_address and port number a server call.
	/// </summary>
	/// <returns>Returns true if binding on m_address was successful, false otherwise.</returns>
	bool bindSocket();

	/// <summary>
	/// Listens to connecting clients, a server call.
	/// </summary>
	/// <returns>Returns true if listening client was successful, false otherwise.</returns>
	bool listenSocket();
};

/// <summary>
/// The class implements a tcp connection with socket.
/// </summary>
class SocketClientTcp : public SocketTCP
{
private:
	bool m_bWasConnected; /* Whether a tranfer connection was established or not */

	/// <summary>
	/// The method checks whether the connection is available.
	/// If not, it tries to reestablish it.
	/// </summary>
	/// <returns>Returns true if the connection can be used now, false otherwise.</returns>
	bool checkConnection();

public:

	/// <summary>
	/// Constructor.
	/// Opens a TCP connection to a remote host.
	/// </summary>
	/// <param name="ip">The ip m_address of the communication partner. If 0, the port will be opened as server.</param>
	/// <param name="port">The port over which will be communicated.</param>
	SocketClientTcp(const char* ip, int port);

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="socketClient">Socket</param>
	SocketClientTcp(int socketClient);

	/// <summary>
	/// Destructor.
	/// </summary>
	~SocketClientTcp();

	/// <summary>
	/// The function sends a block of bytes.
	/// It will return immediately unless the send buffer is full.
	/// </summary>
	/// <param name="buffer">The bytes to send.</param>
	/// <param name="size">The number of bytes to send.</param>
	/// <returns>Returns true if the data was successfully sent, false otherwise.</returns>
	bool send(const char* buffer, int size);

	/// <summary>
	/// The function receives a block of bytes.
	/// </summary>
	/// <param name="buffer">This buffer will be filled with the bytes to receive. It must provide at least "size" bytes.</param>
	/// <param name="size">The number of bytes to receive.</param>
	/// <param name="wait">The method will wait until all bytes have been received. However, it can still fail if the connection is broken.</param>
	/// <returns>Returns true if the data was successfully received, false otherwise.</returns>
	bool receive(char* buffer, int size, bool wait = true);

	/// <summary>
	/// The function receives a block of bytes using the system call recv().
	/// </summary>
	/// <param name="buffer">This buffer will be filled with the bytes to receive.</param>
	/// <param name="size">Size of buffer</param>
	/// <param name="flags">See RECV(2) manpage.</param>
	/// <returns>Number of bytes received, or -1 on error.</returns>
	int receiveSys(void* buffer, unsigned int size, int flags);
};
