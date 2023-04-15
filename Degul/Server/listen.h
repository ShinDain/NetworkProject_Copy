#pragma once
#include "global.h"
#include "Client.h"


DWORD WINAPI ListenerThreadFunc(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터를 받을 소켓과 버퍼
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	while (1)
	{
		if (threadCnt >= MAX_CLIENT_COUNT)
		{
			Sleep(0);
			continue;
		}
		// accept
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		DWORD optval = 1;
		retval = setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY,
			(const char*)&optval, sizeof(optval));

		// 통신 스레드 생성, 소켓 전달
		HANDLE hThread = CreateThread(NULL, 0, ClientThreadFunc, (LPVOID)client_sock, 0, NULL);

		++threadCnt;
	}

	closesocket(listen_sock);
	WSACleanup();

	return 0;
}