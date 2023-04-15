#pragma once
#include "Ball.h"

Ball::Ball() {
  x = 0.0;
  y = 0.0;

  radius = PLAYERRADIUS;
  setRect();

  speedx = 0.0;
  speedy = 0.0;

  count = 0;
  power = 0.0;
}

Ball::Ball(float _x, float _y, int _radius) {
  x = _x;
  y = _y;

  radius = _radius;
  setRect();

  speedx = 0;
  speedy = 0;

  count = 0;
  power = 0.0;
}

BOOL Ball::isCollide(const Ball& b2) {
  float distance;  // distance between 2 centers
  distance = (b2.x - x) * (b2.x - x) + (b2.y - y) * (b2.y - y);
  if (distance < (radius + b2.radius) * (radius + b2.radius)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

void Ball::setRect() {
  rect.left = (LONG)(x - radius);
  rect.top = (LONG)(y - radius);
  rect.right = (LONG)(x + radius);
  rect.bottom = (LONG)(y + radius);
}

