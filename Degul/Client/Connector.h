#pragma once
#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

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
