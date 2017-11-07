#include "UdpComm.h"

#include "../logger/Logger.h"

#include <iostream>
#ifdef WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#undef NOMINMAX
#endif

#else
#  include <cerrno>
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <net/if.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <cstring>
#  include <net/if.h>
#  include <ifaddrs.h>
#  include <stdlib.h> //itoa
#endif

UdpComm::UdpComm()
{
	m_sock = (int)socket(AF_INET, SOCK_DGRAM, 0);
	m_target = (struct sockaddr*)(new struct sockaddr_in);

	if (-1 == m_sock)
		LOGGER_ERROR("Response Packet Client", "Socket invalid");
}

UdpComm::~UdpComm()
{
#ifdef WIN32
	closesocket(m_sock);
#else
	close(m_sock);
#endif

	delete (struct sockaddr_in*)m_target;
}

bool UdpComm::resolve(const char* addrStr, int port, struct sockaddr_in* addr)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	if (1 != inet_pton(AF_INET, addrStr, &(addr->sin_addr.s_addr)))
	{
		LOGGER_ERROR("Response Packet Client", std::string(addrStr) + " is not a valid dotted ipv4 m_address");
		return false;
	}
	return true;
}

bool UdpComm::setTarget(const char* addrStr, int port)
{
	struct sockaddr_in* addr = (struct sockaddr_in*)m_target;
	if (!resolve(addrStr, port, addr))
		return false;
	return true;
}

bool UdpComm::setBlocking(bool block)
{
#ifdef WIN32
	int yes = block ? 0 : 1;
	if (ioctlsocket(m_sock, FIONBIO, (u_long*)&yes))
		return false;
	else
		return true;
#else
	bool result(false);
	if (block)
	{
		if (-1 != fcntl(m_sock, F_SETFL, 0))
			result = true;
	}
	else
	{
		if (-1 != fcntl(m_sock, F_SETFL, O_NONBLOCK))
			result = true;
	}
	return result;
#endif
}

bool UdpComm::setTTL(const char ttl)
{
	int r = setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(unsigned char));
	if (r < 0)
	{
		LOGGER_ERROR("Response Packet Client", std::string("could not set TTL to ") + ttl);
		return false;
	}
	return true;
}

bool UdpComm::setLoopback(bool yesno)
{
	char val = yesno ? 1 : 0;
	int r = setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(char));
	if (r < 0)
	{
		LOGGER_ERROR("Response Packet Client", std::string("could not set ip_multicast_loop to ") + val);
		return false;
	}
	return true;
}

bool UdpComm::joinMulticast(const char* addrStr)
{
	struct sockaddr_in group;
	if (!resolve(addrStr, 0, &group))
		return false;

	//join multicast group for every interface
	if (IN_MULTICAST(ntohl(group.sin_addr.s_addr)))
	{
#ifndef WIN32
		struct ip_mreq mreq;
		struct ifconf ifc;
		struct ifreq* item;
		char buf[1024];

		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = buf;
		if (ioctl(m_sock, SIOCGIFCONF, &ifc) < 0)
		{
			LOGGER_ERROR("Response Packet Client", "cannot get interface list");
			return false;
		}
		else
		{
			bool could_join(false);
			for (unsigned int i = 0; i < ifc.ifc_len / sizeof(struct ifreq); i++)
			{
				item = &ifc.ifc_req[i];
				mreq.imr_multiaddr = group.sin_addr;
				mreq.imr_interface = ((struct sockaddr_in*)&item->ifr_addr)->sin_addr;
				if (0 == setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
					(void*)&mreq, sizeof(mreq)))
				{
					could_join = true;
				}
			}
			if (!could_join)
			{
				LOGGER_ERROR("Response Packet Client", "join multicast group failed for interface");
				return false;
			}
		}
#else
		char host[128];
		struct hostent* pHost;
		if (gethostname(host, sizeof(host)) < 0 || !(pHost = (struct hostent*)gethostbyname(host)))
		{
			LOGGER_ERROR("Response Packet Client", "cannot get interface list");
			return false;
		}

		struct ip_mreq mreq;
		bool couldJoin(false);
		for (int i = 0; pHost->h_addr_list[i]; i++)
		{
			mreq.imr_multiaddr = group.sin_addr;
			mreq.imr_interface = *((struct in_addr*)pHost->h_addr_list[i]);
			if (setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == 0)
				couldJoin = true;
		}
		if (!couldJoin)
		{
			LOGGER_ERROR("Response Packet Client", "join multicast group failed for interface");
			return false;
		}
#endif
		return true;
	}
	else
		LOGGER_ERROR("Response Packet Client", "not a multicast m_address");
	return false;
}

bool UdpComm::setBroadcast(bool enable)
{
	int yes = enable ? 1 : 0;
	if (0 == setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST,
		(const char*)&yes, sizeof(yes)))
	{
		return true;
	}
	else
	{
		char errmsg[256];
		strerror_s(errmsg, 255, errno);
		LOGGER_ERROR("Response Packet Client", std::string("UdpComm::setBroadcast() failed: ") + errmsg);
		return false;
	}
}

bool UdpComm::setRcvBufSize(unsigned int rcvbuf)
{
	if (0 == setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf, sizeof(rcvbuf)))
	{
		char errmsg[256];
		strerror_s(errmsg, 255, errno);
		std::cerr << "multicast-socket: setsockopt for SO_RCVBUF failed: "
			<< errmsg << std::endl;
		return false;
	}

	int result;
	socklen_t result_len = sizeof(result);
	if (0 == getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&result, &result_len))
	{
		std::cerr << "multicast-socket: receive buffer set to "
			<< result << " Bytes." << std::endl;
		return true;
	}

	LOGGER_ERROR("Response Packet Client", "multicast-socket: could not get sockopt SO_RCVBUF");
	return false;
}

bool UdpComm::bind(const char* addr_str, int port)
{
	static const int yes = 1;
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY; //addr.sin_addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

#ifdef BHUMAN_USE_INET_ADDR
	addr.sin_addr.s_addr = inet_addr(addr_str);
#else
	int r = inet_pton(AF_INET, addr_str, &(addr.sin_addr));
	if (r <= 0)
	{
		LOGGER_ERROR("Response Packet Client", std::string("UdpComm::bind() failed: invalid m_address ") + addr_str);
		return false;
	}
#endif

#ifdef SO_REUSEADDR
	if (-1 == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)))
		LOGGER_ERROR("Response Packet Client", "UdpComm: could not set SO_REUSEADDR");
#endif
#ifdef SO_REUSEPORT
	if (-1 == setsockopt(m_sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&yes, sizeof(yes)))
		LOGGER_ERROR("Response Packet Client", "UdpComm: could not set SO_REUSEPORT");
#endif
	if (-1 == ::bind(m_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
	{
		char errmsg[256];
		strerror_s(errmsg, 255, errno);
		LOGGER_ERROR("Response Packet Client", std::string("UdpComm::bind() failed: ") + errmsg);
		return false;
	}

	return true;
}

int UdpComm::read(char* data, int len, unsigned int& ip)
{
	sockaddr_in senderAddr;
#ifdef WIN32
	int size = sizeof(senderAddr);
#else
	unsigned size = sizeof(senderAddr);
#endif
	int result = ::recvfrom(m_sock, data, len, 0, (sockaddr*)&senderAddr, &size);
	if (result <= 0)
		return result;
	ip = ntohl(senderAddr.sin_addr.s_addr);
	return result;
}

int UdpComm::read(char* data, int len)
{
	return ::recv(m_sock, data, len, 0);
}

int UdpComm::readLocal(char* data, int len)
{
	sockaddr_in senderAddr;
#ifdef WIN32
	int size = sizeof(senderAddr);
#else
	unsigned size = sizeof(senderAddr);
	bool found = false;
#endif
	int result = ::recvfrom(m_sock, data, len, 0, (sockaddr*)&senderAddr, &size);
	if (result <= 0)
		return result;
	else
	{
#ifndef WIN32
		struct ifaddrs* addrs, *ifac;

		if (getifaddrs(&addrs) < 0)
			return -1;

		for (ifac = addrs; !found && ifac != NULL; ifac = ifac->ifa_next)
		{
			if (ifac->ifa_flags & IFF_MULTICAST
				&& ifac->ifa_addr
				&& ifac->ifa_addr->sa_family == AF_INET)
			{
				if (((struct sockaddr_in*)ifac->ifa_addr)->sin_addr.s_addr == senderAddr.sin_addr.s_addr)
					found = true;
			}
		}

		freeifaddrs(addrs);

		// no, package comes from the outside -> ignore
		return found ? result : -1;
#else
		return result;
#endif
	}
}

bool UdpComm::write(const char* data, const int len)
{
	return ::sendto(m_sock, data, len, 0,
		m_target, sizeof(struct sockaddr_in)) == len;
}

unsigned short UdpComm::getHost() {
#ifndef WIN32
	struct ifaddrs* ifAddrStruct = NULL;
	struct ifaddrs* ifa = NULL;

	//determine ip m_address
	getifaddrs(&ifAddrStruct);
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
		// manpage getifaddrs    // check it is IP4
		if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET)
		{
			std::string interfaceName(ifa->ifa_name);
			if (interfaceName.find("wlan") != std::string::npos)
			{
				std::string addr = inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr);
				std::string host = addr.substr(addr.rfind(".") + 1);
				return (unsigned short)atoi(host.c_str());
			}
		}
	}
#endif
	return 255;
}