#include "Button.h"

#include "Game.h"
using namespace std;
Button::Button() : size(0), isClicked(false) {}

void Button::initialize(Game& game, int size, CString idleLoc, CString hoverLoc, CString clickLoc, GameState s,
                        Button_Type type) {
  this->size = size;
  stateWhenClicked = s;
  idleImg.Load(idleLoc);
  hoverImg.Load(hoverLoc);
  clickImg.Load(clickLoc);
  this->type = type;
  this->game = &game;
  mouseOldX = mouseNewX = addX = 0;
}

void Button::initialize(Game& game, int size, CString idleLoc, CString hoverLoc, CString clickLoc, Button_Type type) {
  this->size = size;
  idleImg.Load(idleLoc);
  hoverImg.Load(hoverLoc);
  clickImg.Load(clickLoc);
  this->type = type;
  this->game = &game;
  mouseOldX = mouseNewX = -1;
  addX = 0;
}

void Button::draw(HDC hdc, int x, int y, POINT posCursor, bool isDraged, bool isReleased) {
  if (isReleased && !isHovered(posCursor, x + addX, y)) {
    isClicked = false;
  }
  if ((isDraged && isHovered(posCursor, x + addX, y)) || isClicked) {
    isClicked = true;
    if (type == Button_Type::VOLUME_EFFECT || type == Button_Type::VOLUME_BGM) {
      if (mouseOldX == -1) {
        mouseOldX = mouseNewX = posCursor.x;
      } else {
        mouseOldX = mouseNewX;
        mouseNewX = posCursor.x;
        addX += mouseNewX - mouseOldX;
        if (addX < -60) {
          addX = -60;
        } else if (addX > 60) {
          addX = 60;
        }
        // add의 범위는 120
        //-60 ~ 60 -> 0.0 ~ 1.0 또는 8.0
        if (type == Button_Type::VOLUME_EFFECT) {
          game->soundSys.setVolumeEffect((float)(addX + 60) / 20.0f);
        }
          else if (type == Button_Type::VOLUME_BGM) {
          game->soundSys.setVolumeBGM((float)(addX + 60) / 120.0f);
        }
      }
    }
    if (isReleased && isHovered(posCursor, x + addX, y)) {
      isClicked = false;
      
      if (type == Button_Type::CHANGE_GAMESTATE) {
        if (stateWhenClicked == GameState::Lobby) {
          game->startGame();
        }
        else if (stateWhenClicked == GameState::Record) {
          game->AddMessage(MSG_REQ_SCOREBOARD);
        }
        else {
          game->setGameState(stateWhenClicked);
        }
      } else if (type == Button_Type::QUIT) {
        game->quit();
      } else if (type == Button_Type::STARTGAME) {
        if(game->GetIsHost())
          game->AddMessage(MSG_LOBBY_START);
        else
          game->AddMessage(MSG_LOBBY_READY);
      } else if (type == Button_Type::TO_MAINMENU) {
        game->toMainMenu();
      }
      game->soundSys.playEffect(EffectSoundType::click);
    } else if (type == Button_Type::VOLUME_EFFECT || type == Button_Type::VOLUME_BGM) {
      clickImg.Draw(hdc, x + addX, y, size, size);
    } else {
      clickImg.Draw(hdc, x, y, size, size);
    }
  } else if (isHovered(posCursor, x + addX, y)) {
    if (type == Button_Type::VOLUME_EFFECT || type == Button_Type::VOLUME_BGM) {
      hoverImg.Draw(hdc, x + addX, y, size, size);
    } else {
      hoverImg.Draw(hdc, x, y, size, size);
    }
  } else {
    mouseOldX = mouseNewX = -1;
    if (type == Button_Type::VOLUME_EFFECT || type == Button_Type::VOLUME_BGM) {
      idleImg.Draw(hdc, x + addX, y, size, size);
    } else {
      idleImg.Draw(hdc, x, y, size, size);
    }
  }
}

bool Button::isHovered(POINT p, int x, int y) { return (p.x > x && p.x < x + size && p.y > y && p.y < y + size); }

void Button::setstateWhenClicked(GameState state) { stateWhenClicked = state; }
