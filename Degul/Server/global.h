#pragma once
#include "Common.h"
#include <vector>

#include "../Client/DataStruct.h"
#include "../Client/Message.h"

#define BUFSIZE 1002
#define SERVERPORT 9000

#define MAX_CLIENT_COUNT 5
#define MAX_TIMEOUT_CNT 3

// 클라이언트 통신 소켓
SOCKET threadClientSock[MAX_CLIENT_COUNT];

// 이벤트
HANDLE hThreadEvent[MAX_CLIENT_COUNT];
HANDLE hMainEvent[MAX_CLIENT_COUNT];


// 스레드에서 송수신에 사용할 버퍼
char buf[MAX_CLIENT_COUNT][BUFSIZE + 1];
char mainBuf[BUFSIZE + 1];
// 통신 스레드에서 수신한 데이터 크기 저장
short bufLen[MAX_CLIENT_COUNT];
short mainBufLen;

// x번 인덱스를 사용하는 스레드가 있는지 체크
int threadCnt = 0;
int lastThreadidx = 0;
bool threadIndexCheck[MAX_CLIENT_COUNT];
bool threadRecvCheck[MAX_CLIENT_COUNT];
int threadTimeOutCnt[MAX_CLIENT_COUNT];

// 클라이언트 상태 
bool bMainGame = false;
bool bPlayerIn[MAX_CLIENT_COUNT];

// 호스트 인덱스
int hostIdx = -1;

// 구조체 데이터
BallInfo BallData[MAX_CLIENT_COUNT];
BallInfoToClient ToClientBallData[MAX_CLIENT_COUNT];
BallInfoToServer Input[MAX_CLIENT_COUNT];
PlayerInfo PlayerData[MAX_CLIENT_COUNT];
Scoreboard playScoreboard;
Scoreboard HiScoreboard;

// 게임 내 제한시간
float setTime = 60.f;
float playTime = 0.f;
bool bGameEnd = false;


//업데이트

#define ARENA_PIVOT_X 640
#define ARENA_PIVOT_Y 400
#define ARENA_RADIUS 150
#define BALL_RADIUS 12


// 아이템
ItemInfo items[MAX_ITEM_COUNT];
bool itemCreated[MAX_ITEM_COUNT];

std::vector<KillLog_LifeTime> killogs;
std::vector<Chat> chats;

//플레이어 초기 위치
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