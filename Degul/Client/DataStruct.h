#pragma warning(disable:4819)
#pragma once

#define NAME_LENGTH 11
#define MAX_CLIENT_COUNT 5
#define MAX_ITEM_COUNT 10
#define CHAT_LENGTH 25

enum ItemType { weight, lightFeather, powerUp, ID_MAX };

//Client to Server
struct BallInfoToServer {
	float       speedx;
	float       speedy;
};


struct Chat {
	TCHAR     ID[NAME_LENGTH];
	TCHAR     str[CHAT_LENGTH];
};

struct Chat_LifeTime {
	Chat      chat;
	float     lifeTime;
};

//Server to Client
struct BallInfo {
	bool      isAlive;
	TCHAR     id[NAME_LENGTH];
	float     speedx;
	float     speedy;
	float     speedRate;
	float     x;
	float     y;
	float     mass;
};

struct BallInfoToClient
{
	bool      isAlive;
	TCHAR     id[NAME_LENGTH];
	float     x;
	float     y;
};


struct ItemInfo {
	float     x;
	float     y;
	ItemType  itemType;
};

struct KillLog{ // For transfer data
	TCHAR    ID_Killer[NAME_LENGTH];
	TCHAR    ID_Victim[NAME_LENGTH];
};

struct KillLog_LifeTime { // For UI rendering
	KillLog  killLog;
	float    lifeTime;
};

struct Scoreboard {
	TCHAR    ID[MAX_CLIENT_COUNT][NAME_LENGTH];
	int      score[MAX_CLIENT_COUNT];
};

struct ScoreData {
	TCHAR    ID[NAME_LENGTH];
	int      score;
};

struct PlayerInfo {
	TCHAR    ID[NAME_LENGTH];
	bool     isReady;
	bool     isHost;
};