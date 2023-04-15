#pragma warning(disable:4819)
#pragma once
#include "Connector.h"
#include "Game.h"
#include "Message.h"
#include "DataStruct.h"

extern short dataSize;
extern char buf[BUFFER_SIZE];

Game::Game() : UI(UserInterface(*this)) {
  srand((unsigned int)time(NULL));
  //게임 상태 초기화
  nowstate = GameState::MainMenu;
  // 아레나 설정 초기화
  arenaInfo.setArena(arenaType::island);

  this->hWnd = NULL;
  soundSys.playIntro();
  player = balls;

  for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
    scoreboard.ID[i][0] = '\0';
    scoreboard.score[i] = -1;
    players[i].ID[0]    = '\0';
    balls[i].id[0]      = '\0';
    balls[i].x          = GAME_WINDOW_WIDTH / 2.f;
    balls[i].y          = GAME_WINDOW_HEIGHT / 2.f;
    balls_server[i].x   = GAME_WINDOW_WIDTH / 2.f;
    balls_server[i].y   = GAME_WINDOW_HEIGHT / 2.f;
    players[i].isHost   = false;
    players[i].isReady  = false;
  }
}

Game::Game(HWND hWnd) : UI(UserInterface(*this)) {
    srand((unsigned int)time(NULL));
    //게임 상태 초기화
    nowstate = GameState::MainMenu;
    // 아레나 설정 초기화
    arenaInfo.setArena(arenaType::island);

    this->hWnd = hWnd;
    soundSys.playIntro();
    player = balls;

    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
      scoreboard.ID[i][0] = '\0';
      scoreboard.score[i] = -1;
      players[i].ID[0]    = '\0';
      balls[i].id[0]      = '\0';
      balls[i].x          = GAME_WINDOW_WIDTH / 2.f;
      balls[i].y          = GAME_WINDOW_HEIGHT / 2.f;
      balls_server[i].x   = GAME_WINDOW_WIDTH / 2.f;
      balls_server[i].y   = GAME_WINDOW_HEIGHT / 2.f;
      players[i].isHost   = false;
      players[i].isReady  = false;
    }
}

void Game::initID() {
  srand((unsigned int)time(NULL));
  GetRandomID(playerName, 6);
  playerName_caretPos = 6;
}

void Game::initChat() {
  memset(curChat.str, 0, CHAT_LENGTH * sizeof(TCHAR));
  curChat_caretPos = 0;
  _tcscpy_s(curChat.ID, playerName);
}

void Game::initGame() {
    UI.initUI();
    initChat();
    isSaved = false;
    for (Item& item : items) item.isAlive = false;

    // 선택한 아레나 설정
    arenaInfo.setArena(arenaInfo.getArena());
    //플레이어 초기화
    player->x = GAME_WINDOW_WIDTH / 2.f;
    player->y = GAME_WINDOW_HEIGHT / 2.f;
    player->radius = PLAYERRADIUS;
    player->setRect();
    player->speedx = player->speedy = 0;
    player->power = 0;
    player->mass = 1.7f;
    player->isAlive = true;
    isHost = players[playerIndex].isHost;

    //마우스 상태 초기화
    isDraged = false;
    isReleased = false;

    posCursor = { GAME_WINDOW_WIDTH / 2, GAME_WINDOW_HEIGHT / 2 };

    direct.x = direct.y = 0;
    sizeOfDirect = 1;

    isDraged = false;
    InputX = 0;
    InputY = 0;
}

void Game::update(HWND hWnd) {
    eTime = (float)(clock() - lastClock) / (float)CLOCKS_PER_SEC;
    lastClock = clock();

    //std::cout << "\rElapsed Time: "<< eTime << ", " << (int)(1.0f / eTime) << "FPS";

    UI.update(eTime);

    if (isImpulsed) {
        hitPlayer();
    }
    else {
      if ((nowstate == GameState::Lobby && !(posCursor.x > 490 && posCursor.x < 790 && posCursor.y > 500 && posCursor.y < 800)) ||
        nowstate == GameState::Playing) {
        if (isDraged) {
            //공은 항상 화면 중앙에 있기 때문에
            //화면 중앙기준으로 마우스가 어느방향에 있는지 확인
            direct.x = posCursor.x - GAME_WINDOW_WIDTH / 2;
            direct.y = posCursor.y - GAME_WINDOW_HEIGHT / 2;
            sizeOfDirect = sqrt((float)direct.x * direct.x + (float)direct.y * direct.y);
            
            //마우스와 화면상의 공 사이의 거리에 따라서 공의 속도가 달라진다.
            //공과 마우스가 가까울수록 느려진다.
            float speedRatio = 1.0f;
            float SPEED_RATIO_DISTANCE_LIMIT = 100.0f;  //거리가 200 초과면 최고속도가 된다.
            if (sizeOfDirect > SPEED_RATIO_DISTANCE_LIMIT)
                speedRatio = 1.0f;
            else
                speedRatio = sizeOfDirect / SPEED_RATIO_DISTANCE_LIMIT;

            if (sizeOfDirect > 0) {
                InputX = direct.x / sizeOfDirect * speedRatio;
                InputY = direct.y / sizeOfDirect * speedRatio;

            }
        }
        else {
            InputX = InputY = 0;
        }
      }
    }

#ifdef USE_INTERP
    // 공의 이전 위치와 현재 위치를 보간
    for (int i = 0; i < playerNum; i++) {
        balls[i].x = Lerp(balls[i].x, balls_server[i].x, INTERP_SPEED * eTime);
        balls[i].y = Lerp(balls[i].y, balls_server[i].y, INTERP_SPEED * eTime);
        balls[i].setRect();
    }
#endif

    if (!player->isAlive) {
      posCursor = { GAME_WINDOW_WIDTH / 2, GAME_WINDOW_HEIGHT / 2 };
      isImpulsed = false;
      isDraged = false;
    }
}

void Game::hitPlayer() {
    //빠르게 튕겨나기 위해 변수를 설정한다
    direct.x = posCursor.x - GAME_WINDOW_WIDTH / 2;
    direct.y = posCursor.y - GAME_WINDOW_HEIGHT / 2;
    sizeOfDirect = sqrt((float)direct.x * direct.x + (float)direct.y * direct.y);
    if (sizeOfDirect > 0) {
        InputX = player->power * (direct.x / sizeOfDirect) * 150.0f;
        InputY = player->power * (direct.y / sizeOfDirect) * 150.0f;
    }
    else {
        InputX = 0.0;
        InputY = 0.0;
    }
    isDraged = false;
}

void Game::increasePlayerPower() {
    if (player->power < 1.f) {
        player->power += 1.8f * eTime;
    }
}

void Game::updateMouse(bool _isDraged, bool _isReleased, POINT _posCursor) {
    isDraged = _isDraged;
    isReleased = _isReleased;
    posCursor = _posCursor;
}


void Game::draw(HDC hdc, HDC hBufferMemDC) {
    UI.draw(*this, hdc, hBufferMemDC);

  for (Item& item : items) {
    if (item.isAlive)
      UI.drawItem(hBufferMemDC, item);
  }

  for (int i = 0; i < playerNum; i++) {
    if (balls[i].isAlive)
      UI.drawBall(hBufferMemDC, true, balls[i], i);
  }

  //hitbox 그리기
#ifdef DEBUGMOD
    HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN hPen = (HPEN)GetStockObject(BLACK_PEN);
    HPEN hGreenPen = (HPEN)CreatePen(PS_SOLID, 3, RGB(0, 200, 200));
    SelectObject(hBufferMemDC, hbrush);
    HPEN oldPen = (HPEN)SelectObject(hBufferMemDC, hPen);

    for (RECT& hitbox : arenaInfo.safeAreas) {
        Rectangle(hBufferMemDC, hitbox.left, hitbox.top, hitbox.right, hitbox.bottom);
    }
    SelectPen(hBufferMemDC, hGreenPen);
    for (RECT& hitbox : arenaInfo.trapholes) {
        Rectangle(hBufferMemDC, hitbox.left, hitbox.top, hitbox.right, hitbox.bottom);
    }
    SelectPen(hBufferMemDC, oldPen);
    DeletePen(hGreenPen);
#endif
    UI.cameraView(*this, hdc, hBufferMemDC);
}

GameState Game::getGameState() { return nowstate; }

void Game::setGameState(GameState state) {
    nowstate = state;
    soundSys.checkplay();
    switch (nowstate) {
    case GameState::MainMenu:
        soundSys.playIntro();
        break;
    case GameState::Record:
        soundSys.playRun();
        break;
    case GameState::Setting:
        soundSys.playIntro();
        break;
    case GameState::Lobby:
        soundSys.playSelect();
        break;
    case GameState::Playing:
        soundSys.playRun();
        break;
    case GameState::GameOver:
        soundSys.playGameOver();
        break;
    default:
        break;
    }
}

arenaType Game::getArena() { return arenaInfo.getArena(); }

void Game::setArena(int num) { arenaInfo.setArena((arenaType)(num % (int)arenaType::nan)); }

void Game::quit() { PostQuitMessage(0); }

void Game::startGame() {
  AddMessage(MSG_LOBBY_ENTER);
}

int Game::getnHorizonY() { return arenaInfo.nHorizonY; }

const Ball* Game::getPlayer() { return player; }

const POINT& Game::getPosCursor() { return posCursor; }

bool Game::getIsDraged() { return isDraged; }

bool Game::getIsReleased() { return isReleased; }

bool Game::getIsSaved() { return isSaved; }

TCHAR* Game::getPlayerName() { return playerName; }

void Game::typePlayerName(WPARAM wParam) {
    switch (wParam) {
    case VK_BACK:
        if (playerName_caretPos > 0) {
            playerName_caretPos--;
            playerName[playerName_caretPos] = 0;
        }
        break;
    default:
        if (playerName_caretPos < 10 && wParam >= ' ' && wParam <= '~') {
            playerName[playerName_caretPos] = wParam;
            playerName_caretPos++;
        }
        break;
    }
}

void Game::typeChat(WPARAM wParam) {
  switch (wParam) {
  case VK_BACK:
    if (curChat_caretPos > 0) {
      curChat_caretPos--;
      curChat.str[curChat_caretPos] = 0;
    }
    break;
  default:
    if (curChat_caretPos < CHAT_LENGTH - 1 && wParam >= ' ' && wParam <= '~') {
      curChat.str[curChat_caretPos] = wParam;
      curChat_caretPos++;
    }
    break;
  }
}

void Game::toMainMenu() {
    initGame();
    setGameState(GameState::MainMenu);
}

void Game::UpdateFromBuf()
{
    short dataSize_Received = 0;
    char dataInform;

    while (dataSize_Received < dataSize) {
      //Get Message + Data from buf
      memcpy(&dataInform, buf + dataSize_Received, 1);
      dataSize_Received += 1;

      //extract num, flag from dataInform
      int dataNum, flag;
      flag = dataInform & 0x0f;
      dataNum = (dataInform & 0xf0) >> 4;

      switch (flag) {
      case MSG_NOINFO:
        break;
      case MSG_LOBBY_STATUS:
        for (int i = 0;i < dataNum; i++) {
          memcpy(&players[i], buf + dataSize_Received, sizeof(PlayerInfo));
          dataSize_Received += sizeof(PlayerInfo);
          if (players[i].isHost) {
            hostIndex = i;

            // 호스트 권한의 변경을 위해 로비에 입장 후 host를 결정하도록 서버 코드를 수정
            // 그에 따라 로비 내에서 호스트 권한이 변경되므로 ishost변수를 갱신하도록
            // ishost 변수 변경 추가 - 신다인
            isHost = players[playerIndex].isHost;
          }
        }
        playerNum = dataNum;
        for (int i = dataNum; i < MAX_CLIENT_COUNT; i++) {
          balls[i].isAlive = false;
        }
        if (nowstate != GameState::Lobby) {
          initGame();
          setGameState(GameState::Lobby);
        }
        break;
      case MSG_LOBBY_CHAT_TOCLIENT:
      {  
        Chat c;
        for (int i = 0; i < dataNum; i++) {
            memcpy(&c, buf + dataSize_Received, sizeof(Chat));
            dataSize_Received += sizeof(Chat);
            UI.AddChat(c);
        }
      
      break;
      }
      case MSG_PLAYDATA_TOCLIENT:
      BallInfoToClient ball_info;
      for (int i = 0; i < dataNum; i++) {
        memcpy(&ball_info, buf + dataSize_Received, sizeof(ball_info));
        dataSize_Received += sizeof(ball_info);

        //cout << "ball index: " << i << endl;
        //cout << "X:" << ball_info.x << endl;
        //cout << "Y:" << ball_info.y << endl;
        //cout << "speedx:" << ball_info.speedx<< endl;
        //cout << "speedy:" << ball_info.speedy << endl;

        //받은 데이터 순서대로 배열에 저장하기
        balls[i].isAlive = ball_info.isAlive;
        _tcscpy_s(balls[i].id, ball_info.id);

#ifndef USE_INTERP
        //보간 안하기
        balls[i].x = ball_info.x;
        balls[i].y = ball_info.y;
        balls[i].setRect();
#endif
#ifdef USE_INTERP
        //보간하기
        balls_server[i].x = ball_info.x;
        balls_server[i].y = ball_info.y;
#endif
      }
      break;
    case MSG_PLAY_START:
      initGame();
      setGameState(GameState::Playing);
      break;
    case MSG_PLAY_END:
      setGameState(GameState::GameOver);
      break;
    case MSG_PLAY_KILLLOG:
    {
      KillLog k;
      for (int i = 0; i < dataNum; i++) {
        memcpy(&k, buf + dataSize_Received, sizeof(k));
        UI.AddKillLog(k);
        dataSize_Received += sizeof(k);
      }
    }
    break;
    case MSG_PLAY_SCOREBOARD:
      memcpy(&scoreboard, buf + dataSize_Received, sizeof(scoreboard));
      dataSize_Received += sizeof(scoreboard);
      break;
    case MSG_PLAY_TIME:
      memcpy(&playTime, buf + dataSize_Received, sizeof(playTime));
      dataSize_Received += sizeof(playTime);
      break;
    case MSG_PLAY_ITEM:
    {
      ItemInfo info;
      int index = 0;
      for (; index < dataNum; index++) {
        memcpy(&info, buf + dataSize_Received, sizeof(info));
        dataSize_Received += sizeof(info);
        //받은 데이터 순서대로 배열에 저장하기
        items[index].isAlive = true;
        items[index].body.x = info.x;
        items[index].body.y = info.y;
        items[index].itemID = info.itemType;
        items[index].body.setRect();
      }
      //나머지는 아이템이 없는것으로 취급
      for (int i = index; i < ITEM_MAX_COUNT; i++) {
        items[index].isAlive = false;
      }
    }
    break;
   case MSG_SCOREBOARD:
   {
     Scoreboard* score = UI.GetScoreboard();;
     memcpy(score, buf + dataSize_Received, sizeof(Scoreboard));
     dataSize_Received += sizeof(Scoreboard);
     setGameState(GameState::Record);
     break;
   }
    case MSG_SET_INDEX:
      SetPlayer(dataNum);
      initGame();
      break;
    case MSG_ERR_GAME_ONPROGRESS:
      errorCode = MSG_ERR_GAME_ONPROGRESS;
      break;
    default:
      break;
    }
  }
  //cout << dataSize_Received <<"byte / " << dataSize <<"byte" << endl;
} 

void Game::WriteDataToBuf()
{
  switch (nowstate) {
  case GameState::None:
    break;
  case GameState::MainMenu:
    AddMessage(MSG_NOINFO);
    break;
  case GameState::Record:
    break;
  case GameState::Setting:
    break;
  case GameState::Lobby:
    AddMessage(MSG_BALLDATA_TOSERVER);
    break;
  case GameState::Playing:
    AddMessage(MSG_BALLDATA_TOSERVER);
    break;
  case GameState::GameOver:
    break;
  };

  short offset = 0;
  char dataInform;
  int dataNum;
  while (!msgToSend.empty()) {
    char msg = msgToSend.front();
    msgToSend.pop();
    switch (msg) {
    case MSG_LOBBY_ENTER:
      //MSG
      dataInform = msg;
      memcpy(buf + offset, &dataInform, sizeof(dataInform));
      offset += 1;
      //ID
      memcpy(buf + offset, &playerName, sizeof(TCHAR) * NAME_LENGTH);// 2 * 11 = 22 byte
      offset += sizeof(TCHAR) * NAME_LENGTH;
      break;
    case MSG_REQ_SCOREBOARD:
    case MSG_LOBBY_READY:
    case MSG_LOBBY_START:
    case MSG_NOINFO:
      dataInform = msg;
      memcpy(buf + offset, &dataInform, sizeof(dataInform));
      offset += 1;
      break;
    case MSG_LOBBY_CHAT_TOSERVER:
      dataNum = 1;
      dataInform = (dataNum << 4) & 0xf0 | (msg & 0x0f);
      memcpy(buf + offset, &dataInform, sizeof(dataInform));
      memcpy(buf + offset + sizeof(dataInform), &curChat, sizeof(Chat));
      offset += sizeof(Chat) + 1;
      initChat();
      break;
    case MSG_BALLDATA_TOSERVER:
      dataNum = 1;
      BallInfoToServer ball;
      ball.speedx = InputX;
      ball.speedy = InputY;
      if (isImpulsed) {
        isImpulsed = false;
        player->power = 0;
      }
      dataInform = (dataNum << 4) & 0xf0 | (msg & 0x0f);
      memcpy(buf + offset, &dataInform, sizeof(dataInform));
      memcpy(buf + offset + sizeof(dataInform), &ball, sizeof(ball));

      offset += sizeof(ball) + 1;
      break;
    default:
      break;
    }
  }
  dataSize = offset;
}

void Game::SetLastClock(clock_t c)
{
  lastClock = c;
}

void Game::GetRandomID(TCHAR* id, int length)
{
  int i = 0;
  for (; i < length / 2; i++) {
    id[i] = GetRandomChar(false);
  }
   for (; i < length; i++) {
     id[i] = GetRandomNumberChar();
  }
  id[i] = '\0';
}

char Game::GetRandomChar(bool isUpperCase)
{
  int len = 'z' - 'a' + 1;
  char c;
  if (!isUpperCase) {
    c = 'a' + rand() % len;
  }
  else {
    c = 'A' + rand() % len;
  }
  return c;
}

char Game::GetRandomNumberChar()
{
  int len = '9' - '0' + 1;
  char c;

  c = rand() % len;
  c += '0';
  
  return c;
}

void Game::SetPlayer(int index)
{
  playerIndex = index;
  player = &(balls[playerIndex]);
}

void Game::SendChat()
{
  if(lstrlen(curChat.str) > 0)
    AddMessage(MSG_LOBBY_CHAT_TOSERVER);
}

float Game::Lerp(float a, float b, float t)
{
  return a * (1-t) + b * t;
}
