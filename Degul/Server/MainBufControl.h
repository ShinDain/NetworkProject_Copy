#pragma once

void LoadThreadRecvData();
void IntializeClient();
void storeData();

// client_thread�� buf�� �а� ������ �����ϴ� �Լ�
void LoadThreadRecvData()
{
	char bufInfo = 0x00;
	mainBufLen = 0;
	// ���� �����Ͱ� ������ �׻� noinfo�� �ʱ�ȭ
	memset(mainBuf, 0, BUFSIZE + 1);
	memcpy(mainBuf, &bufInfo, sizeof(bufInfo));

	// �ε��� ��ȣ ���� ������ ������ ������ �������� ������ ����
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if (threadIndexCheck[i] && i + 1 > lastThreadidx)
			lastThreadidx = i + 1;
	}

	// ���۸� ��ȸ�ϸ� ������ ����
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		// Ŭ���̾�Ʈ���� ������ �ε����� ���� �б�
		if (threadRecvCheck[i])
		{
			char* threadBuf = buf[i];

			short check = 0;	// ���� ����Ʈ ũ�� üũ

			// ���ۿ� ��� ���̸� ��� ���� ������
			while (bufLen[i] > check)
			{
				// ������ ���� �и� (char��)
				memcpy(&bufInfo, threadBuf + check, sizeof(bufInfo));
				check += sizeof(bufInfo);
				char flag = (bufInfo & 0x0f);			// �޽��� ����
				char num = (bufInfo & 0xf0) >> 4;		// ������ ����

				// �������͸� ���� �ӽ� ���� (���� ����)
				BallInfoToServer clientBall;
				switch (flag)
				{
				case MSG_LOBBY_READY:
					PlayerData[i].isReady = !PlayerData[i].isReady;
					break;
				case MSG_LOBBY_START:
				{
					bool bCanStart = true;

					for (int k = 0; k < lastThreadidx; ++k)
					{
						if (!(PlayerData[k].isReady) && bPlayerIn[k])
							bCanStart = false;
					}
					if (bCanStart)
					{
						playTime = setTime;
						bufInfo = 0x00;
						bufInfo += lastThreadidx;
						bufInfo = bufInfo << 4;
						bufInfo += MSG_PLAY_START;
						memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
						mainBufLen += 1;

						// ���� ��ġ�� �� ��ǥ ����
						IntializeClient();

						bMainGame = true;
					}
					break;
				}
				case MSG_LOBBY_CHAT_TOSERVER:
				{
					Chat tmp;
					memcpy(&tmp, threadBuf + check, sizeof(Chat));
					check += sizeof(Chat);
					chats.push_back(std::move(tmp));
				}
					
					break;
				case MSG_BALLDATA_TOSERVER:
					memcpy(&clientBall, threadBuf + check, sizeof(BallInfoToServer));
					check += sizeof(BallInfoToServer);

					Input[i].speedx = clientBall.speedx;
					Input[i].speedy = clientBall.speedy;
					//printf("[ID:%ws", BallData[i].id);
					//printf("] %f %f\n", clientBall.speedx, clientBall.speedy);
					break;
				}
			}
		}
	}
}

// �׽�Ʈ�� �����͸� ���ۿ� ��� �Լ�
void storeData()
{
	char bufInfo = 0x00;

	// ���� ����
	if (bGameEnd)
	{
		bufInfo += MSG_PLAY_END;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += sizeof(bufInfo);
		return;
	}

	// �κ�
	if (!bMainGame)
	{
		// �÷��̾� ������
		bufInfo = 0x00;
		bufInfo += lastThreadidx;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_LOBBY_STATUS;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += sizeof(bufInfo);
		for (int i = 0; i < lastThreadidx; ++i)
		{
			memcpy(mainBuf + mainBufLen, &PlayerData[i], sizeof(PlayerInfo));
			mainBufLen += sizeof(PlayerInfo);
		}
	}
	// ���� ��
	else if (bMainGame)
	{
		// ������ ������
		int itemcnt = 0;
		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
		{
			if (itemCreated[i])
				++itemcnt;
		}
		bufInfo = 0x00;
		bufInfo += itemcnt;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_PLAY_ITEM;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += 1;
		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
		{
			if (itemCreated[i])
			{
				memcpy(mainBuf + mainBufLen, &items[i], sizeof(ItemInfo));
				mainBufLen += sizeof(ItemInfo);
			}
		}

		// �ð� ������
		bufInfo = 0x00;
		bufInfo += 1;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_PLAY_TIME;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += 1;
		memcpy(mainBuf + mainBufLen, &playTime, sizeof(playTime));
		mainBufLen += sizeof(playTime);

		// ����ǥ ������
		bufInfo = 0x00;
		bufInfo += 1;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_PLAY_SCOREBOARD;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += 1;
		memcpy(mainBuf + mainBufLen, &playScoreboard, sizeof(Scoreboard));
		mainBufLen += sizeof(Scoreboard);


		if (killogs.size() > 0)
		{
			// ų�α� ������
			bufInfo = 0x00;
			bufInfo += killogs.size();
			bufInfo = bufInfo << 4;
			bufInfo += MSG_PLAY_KILLLOG;
			memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
			mainBufLen += 1;
			for (int i = 0; i < killogs.size(); i++)
			{
				memcpy(mainBuf + mainBufLen, &killogs[i], sizeof(KillLog));
				mainBufLen += sizeof(KillLog);
			}
			killogs.clear();
		}
		
	}

	// �� ������
	bufInfo = lastThreadidx;
	//printf("lastThreadidx %d\n", lastThreadidx);
	bufInfo = bufInfo << 4;
	bufInfo += MSG_PLAYDATA_TOCLIENT;
	memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
	mainBufLen += sizeof(bufInfo);
	for (int i = 0; i < lastThreadidx; ++i)
	{
		memcpy(mainBuf + mainBufLen, &ToClientBallData[i], sizeof(BallInfoToClient));
		mainBufLen += sizeof(BallInfoToClient);
	}

	if (chats.size() > 0)
	{
		// ä�� ������
		bufInfo = 0x00;
		bufInfo += chats.size();
		bufInfo = bufInfo << 4;
		bufInfo += MSG_LOBBY_CHAT_TOCLIENT;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += sizeof(bufInfo);

		for (int i = 0; i < chats.size(); i++)
		{
			memcpy(mainBuf + mainBufLen, &chats[i], sizeof(Chat));
			mainBufLen += sizeof(Chat);
		}
		chats.clear();
	}
}

void IntializeClient()
{
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if (bPlayerIn[i])
		{
			BallData[i].speedx = 0;
			BallData[i].speedy = 0;
			BallData[i].isAlive = true;
			BallData[i].x = InitPlayerPos_x[i];
			BallData[i].y = InitPlayerPos_y[i];

			BallInfo temp;
			temp.mass = 0;
			lastCollision[i] = temp;
			lastCollisionIndex[i] = -1;
		}
	}

	for (int i = 0; i < MAX_ITEM_COUNT; i++)
	{
		itemCreated[i] = false;
	}
}