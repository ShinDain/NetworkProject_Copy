#include "global.h"
#include "listen.h"
#include "MainBufControl.h"
#include "Update.h"

void GameEndInit();
void refreshHiscore();

int main(int argv, char* argc[])
{
	FILE* hiscoreFile = fopen("score.dat", "r");

	for (int i = 0; i < 5; ++i)
	{
		fscanf(hiscoreFile, "ID : %ws\tScore : %d\n", HiScoreboard.ID[i], &HiScoreboard.score[i]);
	}
	
	fclose(hiscoreFile);

	// ���� �ʱ�ȭ 
	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		BallData[i].x = -1;
		BallData[i].y = -1;

		threadIndexCheck[i] = false;
		threadRecvCheck[i] = false;
		threadTimeOutCnt[i] = 0;
		hThreadEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hMainEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		bPlayerIn[i] = false;

		playScoreboard.score[i] = -1;

		PlayerData[i].isHost = false;
		PlayerData[i].isReady = false;

		playTime = setTime;
	}

	// ������ ���� ����
	char bufInfo = 0x00;

	// listen ������ ����
	HANDLE hlisten = CreateThread(NULL, 0, ListenerThreadFunc, NULL, 0, NULL);

	

	lastTime = (float)clock();
	// ���� ����
	while (1)
	{
		for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
		{
			threadRecvCheck[i] = false;
		}

		// ������ ���� ���� ����� �ε������� �˻�
		int recvCnt = 0;
		for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
		{
			if (bPlayerIn[i])
			{
				// ȣ��Ʈ ����
				if (hostIdx < 0)
				{
					PlayerData[i].isHost = true;
					PlayerData[i].isReady = true;
					hostIdx = i;
				}

				DWORD res = WaitForSingleObject(hThreadEvent[i], 200);
				if (res == WAIT_TIMEOUT)
				{
					printf("timeout - %d\n", i + 1);
					++threadTimeOutCnt[i];
					if (threadTimeOutCnt[i] > MAX_TIMEOUT_CNT)
					{
						closesocket(threadClientSock[i]);
						BallData[i].isAlive = false;
						printf("[TCP ����] %d�� Ŭ���̾�Ʈ ����", i + 1);

						threadClientSock[i] = NULL;
						threadIndexCheck[i] = false;
						threadTimeOutCnt[i] = 0;
						//playScoreboard.score[i] = -1;
						bPlayerIn[i] = false;

						if (i == hostIdx)
						{
							hostIdx = -1;
							PlayerData[i].isHost = false;
							PlayerData[i].isReady = false;
						}
					}
					continue;
				}

				threadRecvCheck[i] = true;
				threadTimeOutCnt[i] = 0;
			}
		}

		// ���� ���� �޽����� ��� �����尡 ������ ��
		if (bGameEnd)
			bGameEnd = false;

		// recvbuf ������ ����
		LoadThreadRecvData();

		{
			// ���� �ڵ�
			
			//printf("���� ��\n");
			float timeNow = (float)clock();
			float eTime = timeNow - lastTime;
			if (eTime < 30)
			{
				float tmp = 30 - eTime;
 				Sleep(tmp);
			}


			timeNow = clock();
			eTime = timeNow - lastTime;
			eTime /= 1000;
			// ���� ����
			if (bMainGame)
			{
				playTime -= eTime;
				Update(eTime);
				lastTime = timeNow;
			}
			// �κ�
			else
			{
				UpdateBall(eTime);
				CollisionBall(eTime);
				lastTime = timeNow;
			}
			printf("\r%fms, %dFPS", eTime, (int)(1.f/eTime));
				
			/*for(int i = 0 ; i < lastThreadidx; ++i)
				printf("x - %f, y - %f\n", BallData[i].x, BallData[i].y);*/


			for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
			{
				ToClientBallData[i].isAlive = BallData[i].isAlive;
				ToClientBallData[i].x = BallData[i].x;
				ToClientBallData[i].y = BallData[i].y;
			}
			
			int playingCnt = 0;
			for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
			{
				if (bPlayerIn[i])
				{
					++playingCnt;
				}
			}

			if (playTime < FLT_EPSILON || playingCnt < 1)
			{
				bGameEnd = true;
				bMainGame = false;
				playTime = setTime;
				GameEndInit();
			}
			
		}



		// ���ۿ� ������ ���
		storeData();
		
		// ���� �� �۽� ���� ���� �Ϸ� �� set
		for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
		{
		
			if (threadRecvCheck[i])
			{
				SetEvent(hMainEvent[i]);
			}
		}
	}

	return 0;
}

void GameEndInit()
{
	refreshHiscore();

	for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		// Ŭ���̾�Ʈ ������ �ٽ� �ʱ�ȭ
		BallData[i].speedx = 0;
		BallData[i].speedy = 0;
		BallData[i].speedRate = 10;
		BallData[i].mass = 10;
		BallData[i].isAlive = false;
		BallData[i].x = ARENA_PIVOT_X;
		BallData[i].y = ARENA_PIVOT_Y;

		PlayerData[i].isHost = false;
		PlayerData[i].isReady = false;
		playScoreboard.score[i] = -1;
	}
	hostIdx = -1;
	lastThreadidx = -1;
}

// ���̽��ھ� ����
void refreshHiscore()
{
	ScoreData scoreDatas[MAX_CLIENT_COUNT + 5];
	ScoreData tmp;
	int cnt = 0;
	// �ְ��� ������ 
	for (int i = 0; i < 5; ++i)
	{
		if (HiScoreboard.score[i] != -1)
		{
			_tcscpy(tmp.ID, HiScoreboard.ID[i]);
			tmp.score = HiScoreboard.score[i];
			scoreDatas[cnt++] = tmp;
		}
	}
	// �÷��� ������ 
	for(int i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if (playScoreboard.score[i] != -1)
		{
			_tcscpy(tmp.ID, playScoreboard.ID[i]);
			tmp.score = playScoreboard.score[i];
			scoreDatas[cnt++] = tmp;
		}
	}
	// ������ ����
	ScoreData maxScore[5];
	for (int i = 0; i < 5; i++)
	{
		_tcscpy(maxScore[i].ID, L"Score");
		maxScore[i].score = 0;
	}

	int maxIndex = -1;
	int maxValue = -1;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < cnt; j++)
		{
			if (scoreDatas[j].score == 0)continue;
			if (scoreDatas[j].score >= maxValue)
			{
				maxIndex = j;
				maxValue = scoreDatas[j].score;
			}
		}
		if (maxIndex >= 0)
		{
			maxScore[i] = scoreDatas[maxIndex];
			_tcscpy(scoreDatas[maxIndex].ID, L"Score");
			scoreDatas[maxIndex].score = 0;
		}
		maxIndex = -1;
		maxValue = -1;
	}
	/*
	for (int i = 0; i < cnt; ++i)
	{
		for (int j = i; j < cnt; ++j)
		{
			if (scoreDatas[j].score < scoreDatas[j + 1].score)
			{
				tmp = scoreDatas[j];
				scoreDatas[j] = scoreDatas[j + 1];
				scoreDatas[j + 1] = tmp;
			}
		}
	}
	*/

	// ���� ���
	FILE* saveFile = fopen("score.dat", "w");

	for (int i = 0; i < 5; ++i)
	{
		fprintf(saveFile, "ID : %ws\tScore : %d\n", maxScore[i].ID, maxScore[i].score);
		_tcscpy(HiScoreboard.ID[i], maxScore[i].ID);
		HiScoreboard.score[i] = maxScore[i].score;
	}
	fclose(saveFile);
}