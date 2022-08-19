#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "server.h"
#include "../../resource/tool.h"
#include "globaldef.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


static DWORD __stdcall ServerSockConnectHandler(PVOID d) {
	if (!d) return ERROR_INVALID_PARAMETER;

	return 0;
}

DWORD __stdcall mStartServerInstance(PVOID data) {
	if (!data) return ERROR_INVALID_PARAMETER;
	t_mStartServerInstance* cfg = (t_mStartServerInstance*)data;

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = (cfg->ipv6 ? AF_INET6 : AF_INET);
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	auto iResult = getaddrinfo(NULL, std::to_string(cfg->port).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		auto err = WSAGetLastError();
		freeaddrinfo(result);
		return err;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		auto err = WSAGetLastError();
		freeaddrinfo(result);
		closesocket(ListenSocket);
		return err;
	}
	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		auto err = WSAGetLastError();
		closesocket(ListenSocket);
		return err;
	}


	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		auto err = WSAGetLastError();
		closesocket(ListenSocket);
		return err;
	}
	closesocket(ListenSocket);

	constexpr auto buffer_length = 4096;
	mNetwordPackHead* buffer2 = new mNetwordPackHead[buffer_length];
	//char buffer[buffer_length] {};
	volatile auto headers = ((mNetwordPackHead*)buffer2);
	auto& header = headers[0];
	mNetwordPackHead mhead = NPACK_NULL;
	bool verified = false;
	int bytes_received = 0;

	while ((bytes_received = recv(ClientSocket, (char*)buffer2, buffer_length, 0)) > 0) {
		if (!verified) {
			mNetwordPackHead b = NPACK_TYPE_VERIFYPSWD;
			if (cfg->password[0] == 0) {
				verified = true;
				b = NPACK_TYPE_STARTCONNECT;
			}
			//MessageBoxW(0, std::to_wstring(((mNetwordPackHead*)buffer)[0]).c_str(), 0, 0);
			if (header == NPACK_TYPE_PASSWORDSENT) {
#if 0
				std::wfstream fp("debug.txt", std::ios::out);
				fp.write((wchar_t*)(buffer2+2), (mNetwordPackHead)bytes_received-2);
				fp.close();
#endif
				//MessageBoxW(0, (L"Received data " + std::to_wstring(
				//	bytes_received)).c_str(), 0, 0);
				std::wstring str;
				size_t textlen = (size_t)(buffer2 + 1);
				WCHAR* sz = PWSTR(buffer2 + 2);
				//MessageBoxW(0, sz, 0, 0);
				if (0 == wcscmp(cfg->password, sz)) {
					verified = true;
					b = NPACK_TYPE_STARTCONNECT;
				}
				else {
					b = NPACK_TYPE_RETRY;
				}
			}
			send(ClientSocket, (PCSTR)&b, sizeof(b), 0);
		}
	}

	delete[] buffer2;

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		auto err = WSAGetLastError();
		closesocket(ClientSocket);
		return err;
	}

	// cleanup
	closesocket(ClientSocket);


	return 0;
}


