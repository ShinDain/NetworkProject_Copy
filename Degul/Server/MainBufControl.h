#pragma once

void LoadThreadRecvData();
void IntializeClient();
void storeData();

// client_thread의 buf를 읽고 데이터 갱신하는 함수
void LoadThreadRecvData()
{
	char bufInfo = 0x00;
	mainBufLen = 0;
	// 보낼 데이터가 없으면 항상 noinfo로 초기화
	memset(mainBuf, 0, BUFSIZE + 1);
	memcpy(mainBuf, &bufInfo, sizeof(bufInfo));

	// 인덱스 번호 수정 없도록 마지막 접속자 기준으로 데이터 전송
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if (threadIndexCheck[i] && i + 1 > lastThreadidx)
			lastThreadidx = i + 1;
	}

	// 버퍼를 순회하며 데이터 갱신
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		// 클라이언트에서 수신한 인덱스만 버퍼 읽기
		if (threadRecvCheck[i])
		{
			char* threadBuf = buf[i];

			short check = 0;	// 읽은 바이트 크기 체크

			// 버퍼에 담긴 길이를 모두 읽을 때까지
			while (bufLen[i] > check)
			{
				// 데이터 정보 분리 (char형)
				memcpy(&bufInfo, threadBuf + check, sizeof(bufInfo));
				check += sizeof(bufInfo);
				char flag = (bufInfo & 0x0f);			// 메시지 정보
				char num = (bufInfo & 0xf0) >> 4;		// 데이터 개수

				// 공데이터를 받을 임시 변수 (수정 예정)
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

						// 지정 위치로 공 좌표 변경
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

// 테스트용 데이터를 버퍼에 담는 함수
void storeData()
{
	char bufInfo = 0x00;

	// 게임 종료
	if (bGameEnd)
	{
		bufInfo += MSG_PLAY_END;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += sizeof(bufInfo);
		return;
	}

	// 로비
	if (!bMainGame)
	{
		// 플레이어 데이터
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
	// 게임 중
	else if (bMainGame)
	{
		// 아이템 데이터
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

		// 시간 데이터
		bufInfo = 0x00;
		bufInfo += 1;
		bufInfo = bufInfo << 4;
		bufInfo += MSG_PLAY_TIME;
		memcpy(mainBuf + mainBufLen, &bufInfo, sizeof(bufInfo));
		mainBufLen += 1;
		memcpy(mainBuf + mainBufLen, &playTime, sizeof(playTime));
		mainBufLen += sizeof(playTime);

		// 점수표 데이터
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
			// 킬로그 데이터
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

	// 공 데이터
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
		// 채팅 데이터
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