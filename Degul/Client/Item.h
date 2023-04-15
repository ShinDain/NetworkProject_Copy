#pragma warning(disable:4819)
#pragma once
#include "Ball.h"
#include "DataStruct.h"
// 최대 아이템 개수
#define ITEM_MAX_COUNT 10

struct Item {
  bool isAlive = false;
  // 판정 범위
  Ball body;
  //종류
  ItemType itemID;
};
