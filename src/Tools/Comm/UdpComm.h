#pragma once

struct sockaddr;
struct sockaddr_in;

/// <summary>
/// Class implemented Socket UDP.
/// </summary>
class UdpComm
{
public:

	/// <summary>
	/// Constructor.
	/// </summary>
	UdpComm();

	/// <summary>
	/// Destructor.
	/// </summary>
	~UdpComm();

	/// <summary>
	/// Set default m_target m_address.
	/// </summary>
	/// <param name="ip">The ip m_address of the host system.</param>
	/// <param name="port">The port used for the connection.</param>
	/// <returns>Returns true if the connection exist, false otherwise.</returns>
	bool setTarget(const char* ip, int port);

	/// <summary>
	/// Set broadcast mode.
	/// </summary>
	/// <param name="enable">Broadcast mode state.</param>
	/// <returns>Returns true if broadcast mode state was setted successful, false otherwise.</returns>
	bool setBroadcast(bool enable);

	/// <summary>
	/// Set blocking socket.
	/// </summary>
	/// <param name="block">Blocking socket state.</param>
	/// <returns>Returns true if blocking socket UDP state was setted successful, false otherwise.</returns>
	bool setBlocking(bool block);

	/// <summary>
	/// Set multicast mode (please use multicast adresses to avoid confusion).
	/// </summary>
	/// <param name="addrStr">Address string.</param>
	/// <returns>Returns true if multicast mode state was setted successful, false otherwise.</returns>
	bool joinMulticast(const char* addrStr);

	/// <summary>
	/// Set Time-To-Live (router hops).
	/// </summary>
	/// <param name="ttl">Time To Live (TTL)</param>
	/// <returns>Returns true if ttl value was setted successful, false otherwise.</returns>
	bool setTTL(const char ttl);

	/// <summary>
	/// Set Loopback (receive own multicast packets). Default: enabled.
	/// </summary>
	/// <param name="yesno">Loopback state</param>
	/// <returns>Returns true if Loopback state was setted successful, false otherwise.</returns>
	bool setLoopback(bool yesno);

	/// <summary>
	/// Set buffer.
	/// </summary>
	/// <param name="rcvbuf">Size buffer.</param>
	/// <returns>Returns true if size buffer was setted successful, false otherwise.</returns>
	bool setRcvBufSize(unsigned int rcvbuf);

	/// <summary>
	/// Bind to IN_ADDR_ANY to receive packets.
	/// </summary>
	/// <param name="addr">Address</param>
	/// <param name="port">Port</param>
	/// <returns>Returns true if bind address was successful, false otherwise.</returns>
	bool bind(const char* addr, int port);

	/// <summary>
	/// The function tries to read a package from a socket.
	/// </summary>
	/// <param name="data">Buffer data</param>
	/// <param name="len">Size buffer.</param>
	/// <param name="ip">IP remote host.</param>
	/// <returns>Number of bytes received or -1 in case of an error.</returns>
	int read(char* data, int len, unsigned int& ip);

	/// <summary>
	/// The function tries to read a package from a socket.
	/// </summary>
	/// <param name="data">Buffer data.</param>
	/// <param name="len">Size buffer.</param>
	/// <returns>Number of bytes received or -1 in case of an error.</returns>
	int read(char* data, int len);

	/// <summary>
	/// The function tries to read a package from a socket.
	/// It only accepts a package from this host.
	/// </summary>
	/// <param name="data">Buffer data.</param>
	/// <param name="len">Size buffer.</param>
	/// <returns>Number of bytes received or -1 in case of an error.</returns>
	int readLocal(char* data, int len);

	/// <summary>
	/// The function writes a package to a socket.
	/// </summary>
	/// <param name="data">Buffer data.</param>
	/// <param name="len">Size buffer.</param>
	/// <returns>Returns true if the package was written, false otherwise.</returns>
	bool write(const char* data, const int len);

	/// <summary>
	/// Gets Host from ip wlan.
	/// </summary>
	/// <returns>Returns address host.</returns>
	static unsigned short getHost();

private:

	struct sockaddr* m_target; /* Address connection UDP. */

	int m_sock; /* Socket UDP */

	/// <summary>
	/// Resolve DNS address.
	/// </summary>
	/// <param name="addrStr">String address.</param>
	/// <param name="port">Port host.</param>
	/// <param name="addr">Struct address host.</param>
	/// <returns>Returns true if Address was resolved successful, false otherwise.</returns>
	bool resolve(const char* addrStr, int port, struct sockaddr_in* addr);
};
