#pragma once
#include "Common.h"
#include <vector>

#include "../Client/DataStruct.h"
#include "../Client/Message.h"

#define BUFSIZE 1002
#define SERVERPORT 9000

#define MAX_CLIENT_COUNT 5
#define MAX_TIMEOUT_CNT 3

// Ŭ���̾�Ʈ ��� ����
SOCKET threadClientSock[MAX_CLIENT_COUNT];

// �̺�Ʈ
HANDLE hThreadEvent[MAX_CLIENT_COUNT];
HANDLE hMainEvent[MAX_CLIENT_COUNT];


// �����忡�� �ۼ��ſ� ����� ����
char buf[MAX_CLIENT_COUNT][BUFSIZE + 1];
char mainBuf[BUFSIZE + 1];
// ��� �����忡�� ������ ������ ũ�� ����
short bufLen[MAX_CLIENT_COUNT];
short mainBufLen;

// x�� �ε����� ����ϴ� �����尡 �ִ��� üũ
int threadCnt = 0;
int lastThreadidx = 0;
bool threadIndexCheck[MAX_CLIENT_COUNT];
bool threadRecvCheck[MAX_CLIENT_COUNT];
int threadTimeOutCnt[MAX_CLIENT_COUNT];

// Ŭ���̾�Ʈ ���� 
bool bMainGame = false;
bool bPlayerIn[MAX_CLIENT_COUNT];

// ȣ��Ʈ �ε���
int hostIdx = -1;

// ����ü ������
BallInfo BallData[MAX_CLIENT_COUNT];
BallInfoToClient ToClientBallData[MAX_CLIENT_COUNT];
BallInfoToServer Input[MAX_CLIENT_COUNT];
PlayerInfo PlayerData[MAX_CLIENT_COUNT];
Scoreboard playScoreboard;
Scoreboard HiScoreboard;

// ���� �� ���ѽð�
float setTime = 60.f;
float playTime = 0.f;
bool bGameEnd = false;


//������Ʈ

#define ARENA_PIVOT_X 640
#define ARENA_PIVOT_Y 400
#define ARENA_RADIUS 150
#define BALL_RADIUS 12


// ������
ItemInfo items[MAX_ITEM_COUNT];
bool itemCreated[MAX_ITEM_COUNT];

std::vector<KillLog_LifeTime> killogs;
std::vector<Chat> chats;

//�÷��̾� �ʱ� ��ġ
float InitPlayerPos_x[MAX_CLIENT_COUNT] = {
	ARENA_PIVOT_X - 75,
	ARENA_PIVOT_X,
	ARENA_PIVOT_X + 75,
	ARENA_PIVOT_X + 50,
	ARENA_PIVOT_X - 50 
};
float InitPlayerPos_y[MAX_CLIENT_COUNT] = { 
	ARENA_PIVOT_Y,
	ARENA_PIVOT_Y - 50,
	ARENA_PIVOT_Y,
	ARENA_PIVOT_Y + 50,
	ARENA_PIVOT_Y + 50
};

float rebirthTime[5];
BallInfo lastCollision[5];
int lastCollisionIndex[5];
float genItemTime = 0;


float lastTime = 0;