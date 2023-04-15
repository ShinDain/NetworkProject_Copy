#pragma warning(disable:4819)
#pragma once
#include <Windows.h>

#include <vector>

#define GAME_WINDOW_WIDTH 1280
#define GAME_WINDOW_HEIGHT 800
enum class arenaType { island, nan };

class Arena {
 public:
  Arena();

  std::vector<RECT> safeAreas;
  std::vector<RECT> trapholes;
  int nHorizonY;
  void setArena(arenaType type);
  arenaType getArena();

 private:
  arenaType nowType;
};
