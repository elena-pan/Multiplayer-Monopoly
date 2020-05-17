#include "WinSock.h"

namespace Net
{


	struct IP_Endpoint
	{
		uint32 address;
		uint16 port;
	};

	struct Socket
	{
		SOCKET handle;
	};


	static bool init()
	{
		WORD winsock_version = 0x202;
		WSADATA winsock_data;
		if (WSAStartup(winsock_version, &winsock_data))
		{
			printf("WSAStartup failed: %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	static bool socket_create(Socket* out_socket)
	{
		int address_family = AF_INET;
		int type = SOCK_STREAM;
		int protocol = IPPROTO_TCP;
		SOCKET sock = socket(address_family, type, protocol);

		if (sock == INVALID_SOCKET)
		{
			printf("socket failed: %d\n", WSAGetLastError());
			return false;
		}

		// put socket in non-blocking mode
		u_long enabled = 1;
		int result = ioctlsocket(sock, FIONBIO, &enabled);
		if (result == SOCKET_ERROR)
		{
			printf("ioctlsocket failed: %d\n", WSAGetLastError());
			return false;
		}

		*out_socket = {};
		out_socket->handle = sock;
		return true;
	}

	static bool socket_connect(Socket* client_socket, sockaddr_in sockAddr)
	{
		
		int connectResult = connect(client_socket->handle, (struct sockaddr*) & sockAddr, sizeof(sockAddr));

		if (connectResult != 0) {
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK) {
				closesocket(client_socket->handle);
				printf("SERVER UNAVAILABLE, WSAGetLastError %d\n", error);
				return false;
			}
		}
		return true;
		/*
		WSAEVENT NewEvent = WSACreateEvent();
		WSAEventSelect(client_socket->handle, NewEvent, FD_CONNECT | FD_CLOSE);

		int connectResult = connect(client_socket->handle, (struct sockaddr*) & sockAddr, sizeof(sockAddr));

		WSAWaitForMultipleEvents(1, &NewEvent, FALSE, WSA_INFINITE, FALSE);

		WSANETWORKEVENTS connectEvents;
		WSAEnumNetworkEvents(client_socket->handle, NewEvent, &connectEvents);

		if (connectEvents.lNetworkEvents & FD_CONNECT && connectEvents.iErrorCode[FD_CONNECT_BIT] == 0) {
			if (connectResult != 0) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					closesocket(client_socket->handle);
					printf("SERVER UNAVAILABLE, WSAGetLastError %d\n", error);
					return false;
				}
			}
			return true;
		}
		*/
	}

	static bool socket_send(Socket* sock, uint8* packet, uint32 packet_size)
	{
		uint8 prefix = packet_size + 1;
		uint8 buffer[c_socket_buffer_size];
		memcpy(buffer + 1, packet, packet_size);
		buffer[0] = prefix;
		if (send(sock->handle, (const char*)buffer, prefix, 0) == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	static bool socket_receive(Socket* sock, uint8* buffer, uint32 buffer_size, uint32* out_bytes_received)
	{
		int flags = 0;
		int bytes_received = recv(sock->handle, (char*)buffer, buffer_size, flags);

		if (bytes_received == SOCKET_ERROR)
		{
			*out_bytes_received = bytes_received;
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return false;
			}
			return true;
		}

		*out_bytes_received = bytes_received;

		return true;
	}

	static SOCKADDR_IN ip_endpoint_to_sockaddr_in(IP_Endpoint* ip_endpoint)
	{
		SOCKADDR_IN sockaddr_in;
		sockaddr_in.sin_family = AF_INET;
		sockaddr_in.sin_addr.s_addr = htonl(ip_endpoint->address);
		sockaddr_in.sin_port = htons(ip_endpoint->port);
		return sockaddr_in;
	}

	// Client specific
	static IP_Endpoint ip_endpoint_create(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port)
	{
		IP_Endpoint ip_endpoint = {};
		ip_endpoint.address = (a << 24) | (b << 16) | (c << 8) | d;
		ip_endpoint.port = port;
		return ip_endpoint;
	}


} // namespace Net