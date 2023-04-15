#pragma once
#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include <windowsx.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <queue>

using namespace std;

#include "Arena.h"
#include "Ball.h"
#include "EnumState.h"
#include "Item.h"
#include "Sound.h"
#include "UserInterface.h"
#include "resource.h"
#include "Message.h"

#define USE_INTERP
#define INTERP_SPEED 40.f
#define MAX_SCORE_LIST 5

class Connector;

class Game {
public:
  Game();
  Game(HWND);

  void initID();
  void initChat();
  void initGame();

  void update(HWND);
  // 플레이어를 튕길 때
  void hitPlayer();
  // 플레이어의 힘을 증가
  void increasePlayerPower();

  void updateMouse(bool, bool, POINT);

  Sound soundSys;

  void draw(HDC, HDC);

  GameState getGameState();
  void setGameState(GameState);
  void setArena(int num);
  arenaType getArena();
  // void changeVolume(void);
  void quit();
  void startGame();
  int getnHorizonY();
  const Ball* getPlayer();

  const POINT& getPosCursor();
  bool getIsDraged();
  bool getIsReleased();
  bool getIsSaved();
  TCHAR* getPlayerName();
  void typePlayerName(WPARAM);
  void typeChat(WPARAM);
  void toMainMenu();

  void UpdateFromBuf();
  void WriteDataToBuf();

  UserInterface* GetUI() { return &UI; }

  void SetLastClock(clock_t);

  void SetConnector(Connector* c) { connector = c; }
  void SetHwnd(HWND h) { hWnd = h; }

  float GetPlayTime() { return playTime; }
  void SetPlayTime(float t) { playTime = t; }
  int  GetPlayerNum() { return playerNum; }
  void SetPlayerNum(int n) { playerNum = n; }
  int  GetHostIndex() { return hostIndex; }
  void SetHostIndex(int n) { hostIndex = n; }
  bool GetIsImpulsed() { return isImpulsed; }
  void SetIsImpulsed(bool b) { isImpulsed = b; }
  int GetErrorCode() { return errorCode; }
  void SetErrorCode(int c) { errorCode = c; }

  void GetRandomID(TCHAR*, int);
  char GetRandomChar(bool);
  char GetRandomNumberChar();
  bool GetIsHost() { return isHost; }
  Chat* GetChat() { return &curChat; }

  Scoreboard* GetScoreboard() { return &scoreboard; }
  void SetPlayer(int);
  Ball* GetBalls() { return balls; }
  PlayerInfo* GetPlayerInfo() { return players; }

  void AddMessage(char msg) { msgToSend.push(msg); }

  void SendChat();

  float Lerp(float a, float b, float t);

private:
  float eTime; // elapsed Time between frame
  clock_t lastClock;

  bool isSaved;
  TCHAR playerName[11];
  int playerName_caretPos;
  int curChat_caretPos;
  HWND hWnd;
  UserInterface UI;

  GameState nowstate;
  //현재 아레나 정보
  Arena arenaInfo;

  POINT direct;         //움직일 방향 벡터
  float sizeOfDirect;  // direct 벡터의 크기
  bool isImpulsed = false;

  POINT posCursor;
  bool isDraged;
  bool isReleased;

  //아레나에 있는 아이템
  Item items[ITEM_MAX_COUNT];

  float InputX;
  float InputY;
  Ball* player;
  int playerIndex = 0;
  Ball balls[MAX_CLIENT_COUNT];
  Ball balls_server[MAX_CLIENT_COUNT];
  bool isHost;
  int hostIndex = 0;

  PlayerInfo players[MAX_CLIENT_COUNT];
  int playerNum = 0;

  Connector* connector;

  float playTime = 0;
  Scoreboard scoreboard;
  
  Chat curChat;

  queue<char> msgToSend;
  
  int errorCode = -1;
};
