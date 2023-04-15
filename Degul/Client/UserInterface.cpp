#include "UserInterface.h"

#include "Game.h"
//#define DEBUGMODE
UserInterface::UserInterface(Game& game) {
  parentGame = &game;
  
  eTime = 0;
  oldFont = NULL;

  playerView.left = playerView.top = 0;
  playerView.right = playerView.bottom = 1;
  return2This = GameState::MainMenu;

  //아레나 로드
  backgroundImg[(int)arenaType::island].Load(TEXT("Resource\\Arena\\Island.png"));

  //용암 애니메이션 로드
  lavaImg[0].Load(TEXT("Resource\\LavaAnim\\0000.jpg"));
  lavaImg[1].Load(TEXT("Resource\\LavaAnim\\0001.jpg"));
  lavaImg[2].Load(TEXT("Resource\\LavaAnim\\0002.jpg"));
  lavaImg[3].Load(TEXT("Resource\\LavaAnim\\0003.jpg"));
  lavaImg[4].Load(TEXT("Resource\\LavaAnim\\0004.jpg"));
  lavaImg[5].Load(TEXT("Resource\\LavaAnim\\0005.jpg"));
  lavaImg[6].Load(TEXT("Resource\\LavaAnim\\0006.jpg"));
  lavaImg[7].Load(TEXT("Resource\\LavaAnim\\0007.jpg"));
  lavaImg[8].Load(TEXT("Resource\\LavaAnim\\0008.jpg"));
  lavaImg[9].Load(TEXT("Resource\\LavaAnim\\0009.jpg"));
  lavaImg[10].Load(TEXT("Resource\\LavaAnim\\0010.jpg"));
  lavaImg[11].Load(TEXT("Resource\\LavaAnim\\0011.jpg"));
  lavaImg[12].Load(TEXT("Resource\\LavaAnim\\0012.jpg"));
  lavaImg[13].Load(TEXT("Resource\\LavaAnim\\0013.jpg"));
  lavaImg[14].Load(TEXT("Resource\\LavaAnim\\0014.jpg"));
  lavaImg[15].Load(TEXT("Resource\\LavaAnim\\0015.jpg"));
  lavaImg[16].Load(TEXT("Resource\\LavaAnim\\0016.jpg"));
  lavaImg[17].Load(TEXT("Resource\\LavaAnim\\0017.jpg"));
  lavaImg[18].Load(TEXT("Resource\\LavaAnim\\0018.jpg"));
  lavaImg[19].Load(TEXT("Resource\\LavaAnim\\0019.jpg"));

  //공 이미지 로드
  ballImg[0].Load(TEXT("Resource\\Player01.png"));
  ballImg[1].Load(TEXT("Resource\\Player02.png"));
  ballImg[2].Load(TEXT("Resource\\Player03.png"));
  ballImg[3].Load(TEXT("Resource\\Player04.png"));
  ballImg[4].Load(TEXT("Resource\\Player05.png"));

  //아이템 이미지 로드
  itemImg[ItemType::weight].Load(TEXT("Resource\\Item\\weight.png"));
  itemImg[ItemType::lightFeather].Load(TEXT("Resource\\Item\\lightFeather.png"));
  itemImg[ItemType::powerUp].Load(TEXT("Resource\\Item\\powerUp.png"));

  //이펙트 로드
  effectImg.Load(TEXT("Resource\\Effects.png"));

  //현재 재생
  lavaAnimFrame = 0;
  //용암 다음 프레임까지 대기 프레임
  waitingLavaFrame = 0;

  //반투명 이미지
  halfblack.Load(TEXT("Resource\\UI\\HalfBlack.png"));
  halfWhite.Load(TEXT("Resource\\UI\\HalfWhite.png"));

  //버튼 초기화
  buttons_onMainMenu.title.Load(TEXT("Resource\\UI\\DEGUL.png"));
  buttons_onMainMenu.record.initialize(game, 80, "Resource\\UI\\Scoreboard.png", "Resource\\UI\\Scoreboard_Hover.png",
                                       "Resource\\UI\\Scoreboard_Click.png", GameState::Record,
                                       Button_Type::CHANGE_GAMESTATE);
  buttons_onMainMenu.start.initialize(game, 100, "Resource\\UI\\Start.png", "Resource\\UI\\Start_Hover.png",
                                      "Resource\\UI\\Start_Click.png", GameState::Lobby,
                                      Button_Type::CHANGE_GAMESTATE);
  buttons_onMainMenu.setting.initialize(game, 80, "Resource\\UI\\Gear.png", "Resource\\UI\\Gear_Hover.png",
                                        "Resource\\UI\\Gear_Click.png", GameState::Setting,
                                        Button_Type::CHANGE_GAMESTATE);
  buttons_onMainMenu.quit.initialize(game, 40, "Resource\\UI\\Exit.png", "Resource\\UI\\Exit_Hover.png",
                                     "Resource\\UI\\Exit_Click.png", Button_Type::QUIT);

  buttons_onRecord.back.initialize(game, 40, "Resource\\UI\\Arrow.png", "Resource\\UI\\Arrow_Hover.png",
                                   "Resource\\UI\\Arrow_Click.png", GameState::MainMenu, Button_Type::CHANGE_GAMESTATE);

  buttons_onSetting.back.initialize(game, 40, "Resource\\UI\\Arrow.png", "Resource\\UI\\Arrow_Hover.png",
                                    "Resource\\UI\\Arrow_Click.png", GameState::MainMenu, Button_Type::CHANGE_GAMESTATE);
  buttons_onSetting.settingbar.Load(TEXT("Resource\\UI\\SettingBar.png"));
  buttons_onSetting.circle1.initialize(game, 20, "Resource\\UI\\Circle.png", "Resource\\UI\\Circle_Hover.png",
                                       "Resource\\UI\\Circle_Click.png", Button_Type::VOLUME_EFFECT);
  buttons_onSetting.circle2.initialize(game, 20, "Resource\\UI\\Circle.png", "Resource\\UI\\Circle_Hover.png",
                                       "Resource\\UI\\Circle_Click.png", Button_Type::VOLUME_BGM);

  buttons_onLobby.start.initialize(game, 100, "Resource\\UI\\StartGame.png", "Resource\\UI\\StartGame_Hover.png",
                                         "Resource\\UI\\StartGame_Click.png", Button_Type::STARTGAME);
  buttons_onLobby.back.initialize(game, 40, "Resource\\UI\\Arrow.png", "Resource\\UI\\Arrow_Hover.png",
                                        "Resource\\UI\\Arrow_Click.png", GameState::MainMenu,
                                        Button_Type::CHANGE_GAMESTATE);

  buttons_onPlaying.lbuttonImg.Load(TEXT("Resource\\UI\\LButton.png"));

  buttons_onGameOver.record.initialize(game, 80, "Resource\\UI\\Scoreboard.png", "Resource\\UI\\Scoreboard_Hover.png",
                                       "Resource\\UI\\Scoreboard_Click.png", GameState::Record,
                                       Button_Type::CHANGE_GAMESTATE);
  buttons_onGameOver.toLobby.initialize(game, 80, "Resource\\UI\\Retry.png", "Resource\\UI\\Retry_Hover.png",
                                      "Resource\\UI\\Retry_Click.png", GameState::Lobby,
                                      Button_Type::CHANGE_GAMESTATE);
  buttons_onGameOver.setting.initialize(game, 80, "Resource\\UI\\Gear.png", "Resource\\UI\\Gear_Hover.png",
                                        "Resource\\UI\\Gear_Click.png", GameState::Setting,
                                        Button_Type::CHANGE_GAMESTATE);
  buttons_onGameOver.toMainMenu.initialize(game, 40, "Resource\\UI\\Exit.png", "Resource\\UI\\Exit_Hover.png",
                                           "Resource\\UI\\Exit_Click.png", Button_Type::TO_MAINMENU);

  killerLogFont = CreateFont(KILLLOG_FONT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
    VARIABLE_PITCH | FF_ROMAN, NULL);
  chatFont = CreateFont(CHAT_FONT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
    VARIABLE_PITCH | FF_ROMAN, NULL);
}

void UserInterface::update(float eTime) {
  this->eTime = eTime;
  //용암 프레임
  if (waitingLavaFrame < 10) {
    waitingLavaFrame++;
  } else {
    waitingLavaFrame = 0;
    lavaAnimFrame++;
    lavaAnimFrame %= NUM_OF_LAVA_IMAGE;
  }
  UpdateKillLog();
  UpdateChat();
}

void UserInterface::draw(Game& game, HDC hdc, HDC hBufferMemDC) {
  //용암 배경
  lavaImg[lavaAnimFrame].Draw(hBufferMemDC, 0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
  
  //아레나 배경
  backgroundImg[(int)game.getArena()].Draw(hBufferMemDC, 0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
}

void UserInterface::cameraView(Game& game, HDC hDC, HDC hBufferMemDC) {
  HBRUSH hbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
  
  playerView.left = (int)game.getPlayer()->x - GAME_WINDOW_WIDTH / 6;
  playerView.top = (int)game.getPlayer()->y - GAME_WINDOW_HEIGHT / 6;
  playerView.right = playerView.left + GAME_WINDOW_WIDTH / 3;
  playerView.bottom = playerView.top + GAME_WINDOW_HEIGHT / 3;

  int viewWidth = playerView.right - playerView.left;
  int viewWHeight = playerView.bottom - playerView.top;
  int viewCenterX = playerView.left + viewWidth / 2;
  int viewCenterY = playerView.top + viewWHeight / 2;

  //디버그용, 카메라가 보여주는 화면 사각형 좌표를 표시
#ifdef DEBUGMOD
  FrameRect(hBufferMemDC, &playerView, hbrush);
#endif

  //게임 플레이 UI
  //마우스 위치를 카메라뷰 위치로 변환한다.
  POINT posCursor = game.getPosCursor();
  posCursor.x = playerView.left + posCursor.x / 3;
  posCursor.y = playerView.top + posCursor.y / 3;
  bool isDraged = game.getIsDraged();
  bool isReleased = game.getIsReleased();
  SetBkMode(hBufferMemDC, TRANSPARENT);
  SetTextColor(hBufferMemDC, 0x00ffffff);

  switch (game.getGameState()) {
    case GameState::MainMenu:
    {
      buttons_onMainMenu.title.Draw(hBufferMemDC, viewCenterX - 100, playerView.top + 15, 200, 50);
      buttons_onMainMenu.quit.draw(hBufferMemDC, playerView.right - 90, playerView.bottom - 55, posCursor, isDraged,
                                   isReleased);
      TextOut(hBufferMemDC, playerView.right - 50, playerView.bottom - 45, L"Quit", lstrlen(L"Quit"));
      buttons_onMainMenu.record.draw(hBufferMemDC, playerView.left + 40, playerView.top + 80, posCursor, isDraged,
                                     isReleased);
      TextOut(hBufferMemDC, playerView.left + 55, playerView.top + 160, L"Records", lstrlen(L"Records"));
      buttons_onMainMenu.start.draw(hBufferMemDC, viewCenterX - 50, playerView.top + 70, posCursor, isDraged,
                                    isReleased);
      TextOut(hBufferMemDC, viewCenterX - 15, playerView.top + 170, L"Start", lstrlen(L"Start"));
      buttons_onMainMenu.setting.draw(hBufferMemDC, playerView.right - 120, playerView.top + 80, posCursor, isDraged,
                                      isReleased);
      TextOut(hBufferMemDC, playerView.right - 106, playerView.top + 160, L"Settings", lstrlen(L"Settings"));

      //닉네임
      halfblack.Draw(hBufferMemDC, viewCenterX - 50, playerView.bottom - 62, 100, 30);
      TextOut(hBufferMemDC, viewCenterX - 40, playerView.bottom - 55, game.getPlayerName(),
        lstrlen(game.getPlayerName()));  //이름 출력

      return2This = GameState::MainMenu;
    }
      break;
    case GameState::Record: {
      buttons_onRecord.back.setstateWhenClicked(return2This);
      halfblack.Draw(hBufferMemDC, playerView.left, playerView.top + 35, viewWidth, viewWHeight - 70);
      buttons_onRecord.back.draw(hBufferMemDC, playerView.left + 5, playerView.bottom - 50, posCursor, isDraged,
                                 isReleased);
      TextOut(hBufferMemDC, viewCenterX - 40, playerView.top + 40, L"Best Players", lstrlen(L"Best Players"));

      for (int i = 0; i < MAX_SCORE_LIST; i++) {
        if (scoreboard.score[i] < 1) break;
        wstring score = std::to_wstring(scoreboard.score[i]);
        TCHAR rank[5];
        wsprintf(rank, L"%d", i + 1);
        TextOut(hBufferMemDC, viewCenterX - 105, playerView.top + 70 + i * 30, rank, lstrlen(rank));
        TextOut(hBufferMemDC, viewCenterX - 65, playerView.top + 70 + i * 30, scoreboard.ID[i], lstrlen(scoreboard.ID[i]));
        TextOut(hBufferMemDC, viewCenterX + 35, playerView.top + 70 + i * 30, score.c_str(), score.size());
      }
    } break;
    case GameState::Setting:
      buttons_onSetting.back.setstateWhenClicked(return2This);
      halfblack.Draw(hBufferMemDC, playerView.left, playerView.top + 35, viewWidth, viewWHeight - 70);
      buttons_onSetting.back.draw(hBufferMemDC, playerView.left + 5, playerView.bottom - 50, posCursor, isDraged,
                                  isReleased);
      TextOut(hBufferMemDC, viewCenterX - 25, playerView.top + 40, L"Settings", lstrlen(L"Settings"));
      TextOut(hBufferMemDC, viewCenterX - 120, playerView.top + 75, L"Effect Volume", lstrlen(L"Effect Volume"));
      buttons_onSetting.settingbar.Draw(hBufferMemDC, viewCenterX - 25, playerView.top + 9, 150, 150);
      buttons_onSetting.circle1.draw(hBufferMemDC, viewCenterX + 40, playerView.top + 73, posCursor, isDraged,
                                     isReleased);
      TextOut(hBufferMemDC, viewCenterX - 120, playerView.top + 120, L"BGM Volume", lstrlen(L"BGM Volume"));
      buttons_onSetting.settingbar.Draw(hBufferMemDC, viewCenterX - 25, playerView.top + 54, 150, 150);
      buttons_onSetting.circle2.draw(hBufferMemDC, viewCenterX + 40, playerView.top + 118, posCursor, isDraged,
                                     isReleased);

      break;
    case GameState::Lobby:
    {
      buttons_onPlaying.lbuttonImg.Draw(hBufferMemDC, playerView.left + 5, playerView.bottom - 55, 40, 40);
      TextOut(hBufferMemDC, playerView.left + 45, playerView.bottom - 42, L"Move", lstrlen(L"Move"));
      
      buttons_onLobby.start.draw(hBufferMemDC, viewCenterX - 50, playerView.bottom - 100, posCursor, isDraged,
        isReleased);
      if (parentGame->GetIsHost())
        TextOut(hBufferMemDC, viewCenterX - 10, playerView.bottom - 58, L"Start", lstrlen(L"Start"));
      else
        TextOut(hBufferMemDC, viewCenterX - 15, playerView.bottom - 58, L"Ready", lstrlen(L"Ready"));


      TCHAR log[99];
      int offset;
      oldFont = (HFONT)SelectObject(hBufferMemDC, killerLogFont);

      // Player Information
      POINT infoPos = { playerView.left + 10,playerView.top + 5 };
      PlayerInfo* info = parentGame->GetPlayerInfo();
      Ball* balls = parentGame->GetBalls();
      int hostIndex = parentGame->GetHostIndex();
      wsprintf(log, L"Host: %s", info[hostIndex].ID);
      TextOut(hBufferMemDC, infoPos.x + 24, infoPos.y, log, lstrlen(log));
      infoPos.y += 20;
      for (int i = 0; i < parentGame->GetPlayerNum(); i++) {
        Ball& b = balls[i];
        if (!b.isAlive || i == hostIndex) continue;
        wsprintf(log, L"%s", info[i].ID);
        offset = (int)(lstrlen(log) * 6.2f);

        TextOut(hBufferMemDC, infoPos.x - offset + 70, infoPos.y, log, lstrlen(log));
        TextOut(hBufferMemDC, infoPos.x + 70, infoPos.y, L": ", lstrlen(L": "));
        if (info[i].isReady) wsprintf(log, L"Ready");
        else               wsprintf(log, L"Not Ready");
        TextOut(hBufferMemDC, infoPos.x + 80, infoPos.y, log, lstrlen(log));
        infoPos.y += 12;
      }

      DrawID(hBufferMemDC);
      DrawChat(hBufferMemDC);
    }
      break;
    case GameState::Playing:
    {
      buttons_onPlaying.lbuttonImg.Draw(hBufferMemDC, playerView.left + 5, playerView.bottom - 55, 40, 40);
      TextOut(hBufferMemDC, playerView.left + 45, playerView.bottom - 42, L"Move", lstrlen(L"Move"));
      
      //Print Kill Log, 우상단
      oldFont = (HFONT)SelectObject(hBufferMemDC, killerLogFont);
      SetTextColor(hBufferMemDC, 0x00ffffff);
      POINT killLogPos = { playerView.right,playerView.top + 5 };
      TCHAR log[99];
      int offset;
      for (KillLog_LifeTime& k : killLog) {
        wsprintf(log, L"%s", k.killLog.ID_Killer);
        offset = (int)(lstrlen(log) * 6.2f + 110.0f);
        TextOut(hBufferMemDC, killLogPos.x - (int)offset, killLogPos.y, log, lstrlen(log));
        TextOut(hBufferMemDC, killLogPos.x - 100, killLogPos.y, L"->", lstrlen(L"->"));
        wsprintf(log, L"%s", k.killLog.ID_Victim);
        TextOut(hBufferMemDC, killLogPos.x - 80, killLogPos.y, log, lstrlen(log));
        killLogPos.y += 10;
      }

      DrawScoreboard(hBufferMemDC);
      DrawID(hBufferMemDC);
      DrawChat(hBufferMemDC);
      DrawPlayTime(hBufferMemDC);

    }
      break;
    case GameState::GameOver: {
      buttons_onGameOver.record.draw(hBufferMemDC, playerView.left + 40, playerView.top + 80, posCursor, isDraged,
                                     isReleased);
      TextOut(hBufferMemDC, playerView.left + 55, playerView.top + 160, L"Records", lstrlen(L"Records"));
      buttons_onGameOver.toLobby.draw(hBufferMemDC, viewCenterX - 40, playerView.top + 80, posCursor, isDraged,
                                    isReleased);
      TextOut(hBufferMemDC, viewCenterX - 50, playerView.top + 160, L"Return To Lobby", lstrlen(L"Return To Lobby"));
      buttons_onGameOver.setting.draw(hBufferMemDC, playerView.right - 120, playerView.top + 80, posCursor, isDraged,
                                      isReleased);
      TextOut(hBufferMemDC, playerView.right - 106, playerView.top + 160, L"Settings", lstrlen(L"Settings"));
      
      DrawScoreboard(hBufferMemDC);
      return2This = GameState::GameOver;
    } break;
    default:
      break;
  }

#ifndef DEBUGMODE
  StretchBlt(hDC, 0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, hBufferMemDC,
             (int)game.getPlayer()->x - GAME_WINDOW_WIDTH / 6,
             (int)game.getPlayer()->y - GAME_WINDOW_HEIGHT / 6, GAME_WINDOW_WIDTH / 3,
             GAME_WINDOW_HEIGHT / 3,
             SRCCOPY);  //카메라 - 화면을 확대해서 보여줌
#endif
#ifdef DEBUGMODE
  BitBlt(hDC, 0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, hBufferMemDC, 0, 0,
         SRCCOPY);  //카메라 - 전체 화면
#endif
}

void UserInterface::drawBall(HDC hBufferMemDC, bool isEnemy, const Ball& ball, int index) {
  ballImg[index].Draw(hBufferMemDC, ball.rect.left, ball.rect.top, ball.radius * 2, ball.radius * 2, 0, 0, 100, 100);
}

void UserInterface::drawItem(HDC hBufferMemDC, const Item& aItem) {
  // 아이템 타입
  const ItemType& aItemType = aItem.itemID;
  // 아이템 바디
  const Ball& aItemBody = aItem.body;

  itemImg[aItemType].Draw(hBufferMemDC, aItemBody.rect.left, aItemBody.rect.top, aItemBody.radius * 2,
                          aItemBody.radius * 2);
}

void UserInterface::initUI()
{
  killLog.clear();
}

void UserInterface::AddKillLog(KillLog& killlog)
{
  if (killLog.size() >= KILLLOG_MAX_COUNT) {
    killLog.pop_front();
  }
  KillLog_LifeTime k;
  _tcscpy_s(k.killLog.ID_Killer, killlog.ID_Killer);
  _tcscpy_s(k.killLog.ID_Victim, killlog.ID_Victim);

  //char to wchar_t
  /*MultiByteToWideChar(CP_UTF8, 0, k.killLog.ID_Killer, strlen(k.killLog.ID_Killer) + 1,
    k.killer, sizeof(k.killLog.ID_Killer) / sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8, 0, k.killLog.ID_Victim, strlen(k.killLog.ID_Victim) + 1,
    k.victim, sizeof(k.killLog.ID_Victim) / sizeof(wchar_t));*/

  k.lifeTime = 0;

  killLog.push_back(k);
}

void UserInterface::UpdateKillLog()
{
  list<KillLog_LifeTime>::iterator iter;
  for (iter = killLog.begin(); iter != killLog.end();) {
    if (iter->lifeTime > KILLLOG_MAXLIFETIME) {
      iter = killLog.erase(iter);
    }
    else {
      iter->lifeTime += eTime;
      iter++;
    }
  }  
}

void UserInterface::AddChat(Chat& chat)
{
  if (chatList.size() >= CHAT_MAX_COUNT) {
    chatList.pop_front();
  }
  Chat_LifeTime c;
  memcpy(&c.chat, &chat, sizeof(chat));
  c.lifeTime = 0;
  chatList.push_back(c);
}

void UserInterface::UpdateChat()
{
  list<Chat_LifeTime>::iterator iter;
  for (iter = chatList.begin(); iter != chatList.end();) {
    if (iter->lifeTime > KILLLOG_MAXLIFETIME) {
      iter = chatList.erase(iter);
    }
    else {
      iter->lifeTime += eTime;
      iter++;
    }
  }
}

void UserInterface::DrawID(HDC hBufferMemDC)
{
  oldFont = (HFONT)SelectObject(hBufferMemDC, chatFont);
  TCHAR log[99];
  int offset;
  Ball* balls = parentGame->GetBalls();
  //ID 출력
  for (int i = 0; i < parentGame->GetPlayerNum(); i++) {
    Ball& b = balls[i];
    if (balls[i].isAlive) {
      wsprintf(log, L"%s", b.id);
      offset = (int)(lstrlen(log) * 3.1f);
      TextOut(hBufferMemDC, (int)b.x - offset, (int)b.y - 30, log, lstrlen(log));
    }
  }
  SelectObject(hBufferMemDC, oldFont);
}

void UserInterface::DrawChat(HDC hBufferMemDC)
{
  TCHAR log[99];
  int offset, count,line;

  halfblack.Draw(hBufferMemDC, playerView.right - 160, playerView.bottom - 160, 140, 103);
  halfblack.Draw(hBufferMemDC, playerView.right - 160, playerView.bottom - 55, 140, 35);
  oldFont = (HFONT)SelectObject(hBufferMemDC, chatFont);
  SetTextColor(hBufferMemDC, 0x00ffffff);
  POINT chatPos = { playerView.right - 150, playerView.bottom - 155 };
  for (Chat_LifeTime& c : chatList) {
    count = line = offset = 0;
    wsprintf(log, L"%s", c.chat.ID);
    offset = (int)(lstrlen(log) * 6.2f);
    TextOut(hBufferMemDC, chatPos.x, chatPos.y, log, lstrlen(log));
    TextOut(hBufferMemDC, chatPos.x + offset + 10, chatPos.y, L": ", lstrlen(L": "));
    offset += 20;
    for (int i = 0; i < lstrlen(c.chat.str); i++) {
      wsprintf(log, L"%c", c.chat.str[i]);
      TextOut(hBufferMemDC, chatPos.x + offset, chatPos.y, log, lstrlen(log));
      offset += 6;
      count++;
      if ((line == 0 && count > 16 - lstrlen(c.chat.ID)) || (line > 0 && count > 20)) {
        chatPos.y += 12;
        count = 0;
        offset = 0;
        line++;
      }
    }
    chatPos.y += 12;
  }
  TCHAR* str = parentGame->GetChat()->str;
  chatPos.x = playerView.right - 150;
  chatPos.y = playerView.bottom - 50;
  count = offset = 0;
  for (int i = 0; i < lstrlen(str); i++) {
    wsprintf(log, L"%c", str[i]);
    TextOut(hBufferMemDC, chatPos.x + offset, chatPos.y, log, lstrlen(log));
    offset += 6;
    count++;
    if (count > 20) {
      chatPos.y += 12;
      count = 0;
      offset = 0;
    }
  }

  SelectObject(hBufferMemDC, oldFont);
}

void UserInterface::DrawPlayTime(HDC hBufferMemDC)
{
  TCHAR log[99];
  _stprintf_s(log, _countof(log), L"%.2f", parentGame->GetPlayTime());
  LONG center = (playerView.right + playerView.left) / 2;
  TextOut(hBufferMemDC, center, playerView.top + 10, log, lstrlen(log));
}

void UserInterface::DrawScoreboard(HDC hBufferMemDC)
{
  oldFont = (HFONT)SelectObject(hBufferMemDC, killerLogFont);
  SetTextColor(hBufferMemDC, 0x00ffffff);
  TCHAR log[99];
  int offset;

  POINT scoreboardPos = { playerView.left + 10,playerView.top + 5 };
  Scoreboard s = *(parentGame->GetScoreboard());
  wsprintf(log, L"Score");
  TextOut(hBufferMemDC, scoreboardPos.x, scoreboardPos.y, log, lstrlen(log));
  scoreboardPos.y += 12;
  for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if (s.score[i] < 0) continue;
    wsprintf(log, L"%d.", i + 1);
    TextOut(hBufferMemDC, scoreboardPos.x, scoreboardPos.y, log, lstrlen(log));
    wsprintf(log, L"%s", s.ID[i]);
    offset = (int)(lstrlen(log) * 6.2f);
    TextOut(hBufferMemDC, scoreboardPos.x - offset + 70, scoreboardPos.y, log, lstrlen(log));
    TextOut(hBufferMemDC, scoreboardPos.x + 70, scoreboardPos.y, L": ", lstrlen(L": "));
    wsprintf(log, L"%d", s.score[i]);
    TextOut(hBufferMemDC, scoreboardPos.x + 80, scoreboardPos.y, log, lstrlen(log));
    scoreboardPos.y += 12;
  }
  SelectObject(hBufferMemDC, oldFont);
}
