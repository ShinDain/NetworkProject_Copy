#pragma warning(disable:4819)
#pragma once
#define UI_DEBUG
#pragma comment(lib, "msimg32.lib")
#include <Windows.h>
#include <atlimage.h>

#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "Arena.h"
#include "Ball.h"
#include "Button.h"
#include "EnumState.h"
#include "Item.h"
#include "DataStruct.h"

#define GAME_WINDOW_WIDTH 1280
#define GAME_WINDOW_HEIGHT 800

#define KILLLOG_MAXLIFETIME 3.f // sec
#define KILLLOG_FONT_SIZE 11 // sec
#define KILLLOG_MAX_COUNT 5

#define CHAT_MAXLIFETIME 5.f // sec
#define CHAT_FONT_SIZE 11 // sec
#define CHAT_MAX_COUNT 4

const int NUM_OF_LAVA_IMAGE = 20;  //용암 애니메이션 이미지 개수

using namespace std;

//각 상태에 소속되는 버튼을 정의한다.
struct onMainMenu {
  CImage title;
  Button record;
  Button start;
  Button setting;
  Button quit;
};

struct onRecord {
  Button back;
};

struct onSetting {
  CImage settingbar;
  Button circle1, circle2;
  Button back;
};

struct onLobby {
  Button start;
  Button back;
};

struct onPlaying {
  CImage lbuttonImg;
};

struct onGameOver {
  Button record;
  Button toLobby;
  Button setting;
  Button toMainMenu;
};

class UserInterface {
 private:
  Game* parentGame;

  RECT playerView;  //플레이어가 보고있는 화면

  GameState return2This;

  CImage halfblack, halfWhite;
  CImage backgroundImg[3], lavaImg[NUM_OF_LAVA_IMAGE];  //배경
  CImage ballImg[MAX_CLIENT_COUNT], effectImg;         //움직이는 물체
  CImage itemImg[ItemType::ID_MAX];                     // 아이템
  int lavaAnimFrame;                                    //용암 이미지의 현재 프레임
  int waitingLavaFrame;
    
  onMainMenu buttons_onMainMenu;
  onRecord buttons_onRecord;
  onSetting buttons_onSetting;
  onLobby buttons_onLobby;
  onPlaying buttons_onPlaying;
  onGameOver buttons_onGameOver;

  list<KillLog_LifeTime> killLog;
  list<Chat_LifeTime>    chatList;

  float eTime;

  HFONT oldFont;
  HFONT killerLogFont;
  HFONT chatFont;

 public:
  Scoreboard scoreboard;

  UserInterface(Game&);

  void update(float eTime);
  void draw(Game&, HDC, HDC);
  
  void cameraView(Game&, HDC, HDC);

  void drawBall(HDC, bool, const Ball&, int);
  void drawItem(HDC, const Item&);

  void initUI();

  void AddKillLog(KillLog&);
  void UpdateKillLog();

  void AddChat(Chat&);
  void UpdateChat();

  Scoreboard* GetScoreboard() { return &scoreboard; }

  void DrawID(HDC);
  void DrawChat(HDC);
  void DrawPlayTime(HDC);
  void DrawScoreboard(HDC);
};
