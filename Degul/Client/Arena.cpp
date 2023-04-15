#include "Arena.h"

Arena::Arena() {
  nowType = arenaType::island;
  nHorizonY = 0;
}

void Arena::setArena(arenaType type) {
  nowType = type;
  safeAreas.clear();
  trapholes.clear();
  switch (type) {
    case arenaType::island:
      nHorizonY = GAME_WINDOW_HEIGHT / 2;
      // mid box
      safeAreas.push_back({GAME_WINDOW_WIDTH / 2 - 160, GAME_WINDOW_HEIGHT / 2 - 73,  //
                           GAME_WINDOW_WIDTH / 2 + 160, GAME_WINDOW_HEIGHT / 2 + 73});
      // top box
      safeAreas.push_back({GAME_WINDOW_WIDTH / 2 - 120, GAME_WINDOW_HEIGHT / 2 - 110,  //
                           GAME_WINDOW_WIDTH / 2 + 120, GAME_WINDOW_HEIGHT / 2 - 73});
      safeAreas.push_back({GAME_WINDOW_WIDTH / 2 - 80, GAME_WINDOW_HEIGHT / 2 - 140,  //
                           GAME_WINDOW_WIDTH / 2 + 80, GAME_WINDOW_HEIGHT / 2 - 110});
      // bottom box
      safeAreas.push_back({GAME_WINDOW_WIDTH / 2 - 120, GAME_WINDOW_HEIGHT / 2 + 73,  //
                           GAME_WINDOW_WIDTH / 2 + 120, GAME_WINDOW_HEIGHT / 2 + 110});
      safeAreas.push_back({GAME_WINDOW_WIDTH / 2 - 80, GAME_WINDOW_HEIGHT / 2 + 110,  //
                           GAME_WINDOW_WIDTH / 2 + 80, GAME_WINDOW_HEIGHT / 2 + 140});
      break;
    default:
      break;
  }
}

arenaType Arena::getArena() { return nowType; }
