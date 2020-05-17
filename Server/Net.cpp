#include "WinSock.h"

extern bool recvError;

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

		// Allow for immediate reuse of port
		int i = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));

		// t socket in non-blocking mode
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

	static bool socket_accept(Socket* sock, Socket* out_client_socket, sockaddr_in* address)
	{

		SOCKET client_socket = accept(sock->handle, (sockaddr*)address, NULL);

		if (client_socket == INVALID_SOCKET) {
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK) {
				printf("accept failed with error: %d\n", WSAGetLastError());
			}
			
			return false;
		}

		*out_client_socket = {};
		out_client_socket->handle = client_socket;

		return true;
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

	// Server specific
	static bool socket_bind(Socket* sock, IP_Endpoint* local_endpoint)
	{
		SOCKADDR_IN local_address = ip_endpoint_to_sockaddr_in(local_endpoint);
		if (bind(sock->handle, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
		{
			printf("bind failed: %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

} // namespace Net