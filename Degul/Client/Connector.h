#pragma once
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

#define SERVERINFO_PATH "./Resource/serverinfo.txt"
#define TIMEOUT_MSEC 2000
#include "Message.h"
#include "ErrorTeller.h"
#include "Game.h"

class Connector {
public:
	Connector();

	void GetIPFromFile();
	bool ConnectToServer();
	void CloseClient();

	void SendToServer();
	void ReceiveFromServer();

	void SetMessengerThreadHandle(HANDLE h) { messengerThread = h; }

	void SetGame(Game* g) { game = g; };

private:
	Game* game;

	ErrorTeller et;

	WSADATA wsa;
	SOCKET sock;

	char serverIP[32];
	int serverPort;

	bool isClosed = false;

	HANDLE messengerThread;
};
