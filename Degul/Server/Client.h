#pragma once

#include "global.h"
#include <string>
#include <time.h>

DWORD WINAPI ClientThreadFunc(LPVOID arg)
{
	// Ŭ���̾�Ʈ ����, ������ �ʱ�ȭ
	// �����尡 ����� ������ �ε��� ����
	int Index = 0;
	while (threadIndexCheck[Index])
		++Index;

	char* threadBuf = buf[Index];
	threadIndexCheck[Index] = true;

	SOCKET client_sock = (SOCKET)arg;
	int retval;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	// ������ Ŭ���̾�Ʈ ����
	threadClientSock[Index] = client_sock;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("\n[TCP ����] Ŭ���̾�Ʈ ���� : IPv4 �ּ� = %s, ��Ʈ ��ȣ = %d\n",
		addr, ntohs(clientaddr.sin_port));

	// �� ������ ���Ƿ� �ʱ�ȭ (�ӽ�)
	srand(time(NULL));
	TCHAR tStr[5];
	for (int i = 0; i < 4; ++i)
		tStr[i] = 'a' + (rand() % 26);
	tStr[4] = '\0';
	_tcscpy(BallData[Index].id, tStr);

	BallData[Index].speedx = 0;
	BallData[Index].speedy = 0;
	BallData[Index].speedRate = 10;
	BallData[Index].mass = 10;
	BallData[Index].isAlive = false;
	BallData[Index].x = -1;
	BallData[Index].y = -1;
	_tcscpy(playScoreboard.ID[Index] , tStr);
	_tcscpy(PlayerData[Index].ID, tStr);
	
	PlayerData[Index].isReady = false;
	PlayerData[Index].isHost = false;

	// �������� ������ ����
	short* len = &bufLen[Index];

	char bufInfo = 0x00;

	// ���� ���� �ʱ� �ۼ���
	{
		// MSG_SET_INDEX ���� (�ε��� ���� ����)
		bufInfo += Index;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_SET_INDEX;
		memcpy(threadBuf, &bufInfo, sizeof(bufInfo));
		
		short tmplen = sizeof(bufInfo);
		retval = send(client_sock, (char*)&tmplen, sizeof(short), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()1");
			closesocket(client_sock);
			threadClientSock[Index] = NULL;
			printf("[TCP ����] Ŭ���̾�Ʈ ���� : IPv4 = %s, ��Ʈ ��ȣ = %d \n",
				addr, ntohs(clientaddr.sin_port));

			threadIndexCheck[Index] = false;
			threadTimeOutCnt[Index] = 0;
			--threadCnt;
			return -1;
		}

		retval = send(client_sock, (char*)threadBuf, tmplen, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()2");
			closesocket(client_sock);
			threadClientSock[Index] = NULL;
			printf("[TCP ����] Ŭ���̾�Ʈ ���� : IPv4 = %s, ��Ʈ ��ȣ = %d \n",
				addr, ntohs(clientaddr.sin_port));

			threadIndexCheck[Index] = false;
			threadTimeOutCnt[Index] = 0;
			--threadCnt;
			return -1;
		}
	}

	// ������ �ۼ��� ����
	while (1)
	{
		// ������ ���۷� ������ ����
		retval = recv(client_sock, (char*)len, sizeof(short), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()1");
			break;
		}
		else if (retval == 0)
			break;
		retval = recv(client_sock, threadBuf, *len, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()2");
			break;
		}
		else if (retval == 0)
			break;

		threadBuf[retval] = '\0';

		// ������ ������ üũ
		int check = 0;
		char flag = -1;
		char num = -1;
		short tmplen = 0;

		// Ÿ��Ʋ, ���â ���� �޽��� �ۼ���, ������ ���۸��� ����� �ۼ����Ѵ�.
		if (!bPlayerIn[Index])
		{
			bool bData = false;

			bufInfo = 0x00;
			memcpy(&bufInfo, threadBuf + check, sizeof(bufInfo));
			flag = bufInfo & 0x0f;
			num = (bufInfo & 0xf0) << 4;
			check += sizeof(bufInfo);

			// ��� �����忡�� �������� ������ ����
			switch (flag)
			{
			case MSG_REQ_SCOREBOARD:
				// �ְ� ��� �����͸� ��´�.
				bufInfo = 0x00;
				bufInfo += 0;
				bufInfo = bufInfo << 4;
				bufInfo += MSG_SCOREBOARD;
				memcpy(threadBuf + tmplen, &bufInfo, sizeof(bufInfo));
				tmplen += sizeof(bufInfo);

				memcpy(threadBuf + tmplen, &HiScoreboard, sizeof(Scoreboard));
				tmplen += sizeof(Scoreboard);
				bData = true;
				break;
			case MSG_LOBBY_ENTER:
				if (!bMainGame)
				{
					TCHAR tmp[NAME_LENGTH];
					memcpy(tmp, threadBuf + check, sizeof(TCHAR)* NAME_LENGTH);
					check += sizeof(TCHAR) * NAME_LENGTH;
					
					_tcscpy(playScoreboard.ID[Index], tmp);
					_tcscpy(PlayerData[Index].ID, tmp);
					_tcscpy(BallData[Index].id, tmp);
					_tcscpy(ToClientBallData[Index].id, tmp);

					bPlayerIn[Index] = true;
					BallData[Index].isAlive = true;
					//BallData[Index].x = ARENA_PIVOT_X;
					//BallData[Index].y = ARENA_PIVOT_Y;
					playScoreboard.score[Index] = 0;

					bufInfo = 0x00;
					bufInfo += lastThreadidx;
					bufInfo = bufInfo << 4;
					bufInfo += MSG_LOBBY_STATUS;
					memcpy(threadBuf + tmplen, &bufInfo, sizeof(bufInfo));
					tmplen += sizeof(bufInfo);
					for (int k = 0; k < lastThreadidx; ++k)
					{
						memcpy(threadBuf + tmplen, &PlayerData[k], sizeof(PlayerInfo));
						tmplen += sizeof(PlayerInfo);
					}
					bData = true;
				}
				else
				{
					bufInfo = 0x00;
					bufInfo += 0;
					bufInfo = bufInfo << 4;
					bufInfo += MSG_ERR_GAME_ONPROGRESS;
					memcpy(threadBuf + tmplen, &bufInfo, sizeof(bufInfo));
					tmplen += sizeof(bufInfo);
					bData = true;
				}
				break;
			}
			
			// ���� �����Ͱ� ������ noinfo��
			if (!bData)
			{
				bufInfo = 0x00;
				bufInfo += MSG_NOINFO;
				memcpy(threadBuf, &bufInfo, sizeof(bufInfo));
				tmplen += sizeof(bufInfo);
			}
		}
		// �κ�, �÷��� ���� �޽��� �ۼ���, ���� ������ �����͸� �ۼ����Ѵ�.
		else
		{
			// ������ ���� ������ main�� �˸���.
			SetEvent(hThreadEvent[Index]);

			// main�� ���� ���Ḧ ��ٸ���.
			WaitForSingleObject(hMainEvent[Index], INFINITE);

			if (bGameEnd)
				bPlayerIn[Index] = false;

			// main ���� ����
			memcpy(threadBuf + tmplen, mainBuf, mainBufLen);
			tmplen += mainBufLen;
		}

		retval = send(client_sock, (char*)&tmplen, sizeof(short), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()3");
			break;
		}

		retval = send(client_sock, (char*)threadBuf, tmplen, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()4");
			break;
		}
	}

	// Ŭ���̾�Ʈ ���� ���� ������ �ʱ�ȭ
	closesocket(client_sock);
	threadClientSock[Index] = NULL;
	printf("[TCP ����] Ŭ���̾�Ʈ ���� : IPv4 = %s, ��Ʈ ��ȣ = %d \n",
		addr, ntohs(clientaddr.sin_port));


	if (Index == hostIdx)
	{
		hostIdx = -1;
		PlayerData[Index].isHost = false;
		PlayerData[Index].isReady = false;
	}

	BallData[Index].isAlive = false;
	//playScoreboard.score[Index] = -1;

	threadIndexCheck[Index] = false;
	threadTimeOutCnt[Index] = 0;
	bPlayerIn[Index] = false;
	--threadCnt;
	return 0;
}