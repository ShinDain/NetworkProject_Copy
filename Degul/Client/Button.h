#pragma warning(disable:4819)
#pragma once
#pragma comment(lib, "msimg32.lib")
#include <Windows.h>
#include <atlimage.h>

#include <iostream>

#include "EnumState.h"

enum class Button_Type { STARTGAME, CHANGE_GAMESTATE, VOLUME_EFFECT, VOLUME_BGM, QUIT, TO_MAINMENU};

class Game;

class Button {
 private:
  int mouseOldX, mouseNewX;
  Button_Type type;
  CImage idleImg, hoverImg, clickImg;
  int size;
  bool isClicked;
  GameState stateWhenClicked;  //클릭당했을때 게임의 state를 이것으로 만든다.
  Game* game;
  int addX;

 public:
  Button();
  void initialize(Game&, int, CString idleLoc, CString hoverLoc, CString clickLoc, GameState, Button_Type);
  void initialize(Game&, int, CString idleLoc, CString hoverLoc, CString clickLoc, Button_Type);

  void draw(HDC, int x, int y, POINT mousePos, bool isDraged, bool isReleased);

  bool isHovered(POINT, int, int);
  void setstateWhenClicked(GameState);
};
