#pragma once
#include "Connector.h"
#include "Message.h"
#include "DataStruct.h"
#include <iostream>
#include <fstream>
using namespace std;

extern short dataSize;
extern char buf[BUFFER_SIZE];
extern HANDLE MainReadEvent;

Connector::Connector()
{
    memset(&wsa, 0, sizeof(wsa));
    this->sock = NULL;

    memset(&serverIP, 0, sizeof(serverIP));
    serverPort = 0;

    dataSize = 0;
}

void Connector::GetIPFromFile()
{
    ifstream file;
    file.open(SERVERINFO_PATH);
    if (!file.is_open()) {
        char msg[256] = "Unable to open ";
        strcat_s(msg, SERVERINFO_PATH);
        et.ShowMsg(msg);
        serverIP[0] = -1;
        return;
    }
    file >> serverIP;
    file >> serverPort;
}

bool Connector::ConnectToServer()
{
    int result, retval;
    GetIPFromFile();
    if ((int)serverIP[0] == -1) {
        et.ShowMsg("Server IP initialization failed");
        return false;
    }
    result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        et.ShowLastErrMsg("WSAStartup()");
        return false;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        et.ShowLastErrMsg("socket()");
        return false;
    }

#ifdef _DEBUG
    cout << "Try to connect Server - IP: " << serverIP << ", Port: " << serverPort << endl;
#endif

    DWORD optval;

    optval = 1;
    retval = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
      (const char*)&optval, sizeof(optval));

    optval = TIMEOUT_MSEC;
    retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
      (const char*)&optval, sizeof(optval));

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(serverPort);
    retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        et.ShowLastErrMsg("connect()");
        closesocket(sock);
        return false;
    }

#ifdef _DEBUG
    cout << "Connected" << endl;
#endif
    return true;
}

void Connector::CloseClient()
{
    isClosed = true;
    closesocket(sock);
    WSACleanup();
}

void Connector::SendToServer()
{
    if (isClosed) return;

    //check exception
    if (dataSize <= 0) {
        dataSize = 1;
        memset(buf, MSG_NOINFO, 1);
    }

    int retval;
    //Send Data size
    retval = send(sock, (char*)&dataSize, sizeof(dataSize), 0);
    if (retval == SOCKET_ERROR && !isClosed) {
      game->setGameState(GameState::None);
      et.ShowLastErrMsg("send data size");
      CloseClient();
      exit(1);
    }
    //Send Message + Data
    retval = send(sock, buf, dataSize, 0);
    if (retval == SOCKET_ERROR && !isClosed) {
      game->setGameState(GameState::None);
      et.ShowLastErrMsg("send data");
      CloseClient();
      exit(1);
    }
}

void Connector::ReceiveFromServer()
{
    if (isClosed) return;

    int retval;
    retval = recv(sock, (char*)&dataSize, sizeof(dataSize), MSG_WAITALL);
    if (retval == SOCKET_ERROR && !isClosed) {
      game->setGameState(GameState::None);
      et.ShowLastErrMsg("recv data size");
      CloseClient();
      exit(1);
    }
    retval = recv(sock, buf, dataSize, MSG_WAITALL);
    if (retval == SOCKET_ERROR && !isClosed) {
      game->setGameState(GameState::None);
      et.ShowLastErrMsg("recv data");
      CloseClient();
      exit(1);
    }
}
