#pragma warning(disable:4819)
#pragma once
#include <windows.h>

#define NAME_LENGTH 11
#define PLAYERSPEED 8
#define PLAYERRADIUS 12

class Ball {
 public:
  Ball();
  Ball(float _x, float _y, int _radius);
  float x, y;
  int radius;
  RECT rect;
  //속도
  float speedx, speedy;
  //파워게이지
  float power;
  //무게
  float mass;
  int count;
  bool isAlive = false;
  TCHAR id[NAME_LENGTH];

  BOOL isCollide(const Ball& b2);
  void setRect();  //공의 x, y좌표를 토대로 rect(그려져야 하는 범위)를 정해줌
};
