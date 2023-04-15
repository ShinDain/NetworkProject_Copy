#pragma warning(disable:4819)
#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

#define _WINSOCKAPI_
#include <Windows.h>
#include <atlimage.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <windowsx.h>

#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Game.h"
#include "Connector.h"
#include "Message.h"

#define GAME_WINDOW_WIDTH 1280
#define GAME_WINDOW_HEIGHT 800

HINSTANCE g_hlnst;
LPCTSTR lpszClass = TEXT("Window Class Name");
LPCTSTR lpszWindowName = TEXT("Degul");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI Messenger(LPVOID arg);
void showError(int);

short dataSize;
char buf[BUFFER_SIZE];
HANDLE MainReadEvent;
Connector degul_connector;
ErrorTeller et;
Game degulgame;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
  HWND hWnd;
  MSG Message;
  WNDCLASSEX WndClass;
  g_hlnst = hInstance;

  WndClass.cbSize = sizeof(WndClass);
  WndClass.style = CS_HREDRAW | CS_VREDRAW;
  WndClass.lpfnWndProc = (WNDPROC)WndProc;
  WndClass.cbClsExtra = 0;
  WndClass.cbWndExtra = 0;
  WndClass.hInstance = hInstance;
  WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
  WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  WndClass.lpszMenuName = NULL;
  WndClass.lpszClassName = lpszClass;
  WndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  RegisterClassEx(&WndClass);

  hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    (1920 - GAME_WINDOW_WIDTH) / 2, (1080 - GAME_WINDOW_HEIGHT) / 2, GAME_WINDOW_WIDTH,
    GAME_WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  MainReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
  if (!degul_connector.ConnectToServer()) return 0;

  HANDLE hThread = CreateThread(NULL, 0, Messenger,
    (LPVOID)&degul_connector, 0, NULL);
  if (hThread == NULL) {
    degul_connector.CloseClient();
    return 0;
  }
  else { CloseHandle(hThread); }

  degul_connector.SetMessengerThreadHandle(hThread);
  et.SetHwnd(hWnd);
  degulgame.SetHwnd(hWnd);


  while (GetMessage(&Message, 0, 0, 0)) {
    TranslateMessage(&Message);
    DispatchMessage(&Message);
  }
  degul_connector.CloseClient();
  CloseHandle(MainReadEvent);
  return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hDC, hBufferMemDC;
  // buffer memDC
  HBITMAP oldBitmapBuffer;

  // handle loaded bitmap
  static HBITMAP hBufferBitmap;

  // mouse cursor
  static POINT posCursor = { 0, 0 };

  static bool isStarted = false;
  static bool isDraged = false;
  static bool isReleased = false;

  showError(degulgame.GetErrorCode());

  switch (iMessage) {
  case WM_COMMAND:
    break;
  case WM_LBUTTONDOWN:
    if (degulgame.getGameState() == GameState::None) break;
    if (!isDraged) {
      posCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      isDraged = true;
      isReleased = false;
      degulgame.updateMouse(isDraged, isReleased, posCursor);
      if ((degulgame.getGameState() == GameState::Lobby && !(posCursor.x > 490 && posCursor.x < 790 && posCursor.y > 500 && posCursor.y < 800)) ||
        degulgame.getGameState() == GameState::Playing)
      {
          SetTimer(hWnd, 2, 10, NULL);  // power 증가
      }
    }
    break;
  case WM_LBUTTONUP:
    if (degulgame.getGameState() == GameState::None) break;
    if (isDraged) {
      KillTimer(hWnd, 2);
      posCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      isDraged = false;
      isReleased = true;
      degulgame.updateMouse(isDraged, isReleased, posCursor);
      if (degulgame.getGameState() == GameState::Playing ||
        degulgame.getGameState() == GameState::Lobby) {
        degulgame.SetIsImpulsed(true);
        degulgame.soundSys.playEffect(EffectSoundType::hit);
      }
    }
    break;
  case WM_MOUSEMOVE: {
    POINT nowCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    degulgame.updateMouse(isDraged, isReleased, nowCursor);
  } break;
  case WM_KEYDOWN:
    if (degulgame.getGameState() == GameState::None) break;
    switch (wParam) {
    case VK_CONTROL:
      if (!isDraged) {
        GetCursorPos(&posCursor);
        isDraged = true;
        isReleased = false;
        degulgame.updateMouse(isDraged, isReleased, posCursor);
        if ((degulgame.getGameState() == GameState::Lobby && !(posCursor.x > 490 && posCursor.x < 790 && posCursor.y > 500 && posCursor.y < 800)) ||
          degulgame.getGameState() == GameState::Playing)
        {
          SetTimer(hWnd, 2, 10, NULL);  // power 증가
        }
      }
      break;
    case VK_RETURN:
      if (degulgame.getGameState() == GameState::Lobby ||
        degulgame.getGameState() == GameState::Playing ||
        degulgame.getGameState() == GameState::GameOver) {
        degulgame.SendChat();
      }
      break;
    }
    break;
  case WM_KEYUP:
    if (degulgame.getGameState() == GameState::None) break;
    switch (wParam) {
    case VK_CONTROL:
      if (isDraged) {
        KillTimer(hWnd, 2);
        GetCursorPos(&posCursor);
        isDraged = false;
        isReleased = true;
        degulgame.updateMouse(isDraged, isReleased, posCursor);
        if (degulgame.getGameState() == GameState::Playing ||
          degulgame.getGameState() == GameState::Lobby) {
          degulgame.SetIsImpulsed(true);
          degulgame.soundSys.playEffect(EffectSoundType::hit);
        }
      }
      break;
    }
    break;
  case WM_CHAR:
    if (degulgame.getGameState() == GameState::MainMenu) {
      degulgame.typePlayerName(wParam);
    }
    if (degulgame.getGameState() == GameState::Lobby ||
        degulgame.getGameState() == GameState::Playing || 
        degulgame.getGameState() == GameState::GameOver ) {
      degulgame.typeChat(wParam);
    }
#ifdef _DEBUG
    // Add Random KillLog
    if (wParam == 'a') {
      KillLog k;
      degulgame.GetRandomID(k.ID_Killer, rand() % 9 + 2);
      degulgame.GetRandomID(k.ID_Victim, rand() % 9 + 2);

      degulgame.GetUI()->AddKillLog(k);
    }
    // Add Random Chat Data
    if (wParam == 's' && 
      (degulgame.getGameState() == GameState::Lobby ||
       degulgame.getGameState() == GameState::Playing ||
       degulgame.getGameState() == GameState::GameOver)) {
      Chat c;
      memcpy(c.ID, degulgame.getPlayerName(), sizeof(TCHAR)*NAME_LENGTH);
      degulgame.GetRandomID(c.str, CHAT_LENGTH - 1);

      degulgame.GetUI()->AddChat(c);
    }
#endif
    break;
  case WM_TIMER:
    if (degulgame.getGameState() == GameState::None) break;
    switch (wParam) {
    case 1: // Update
    {
      DWORD retval;
      retval = WaitForSingleObject(MainReadEvent, INFINITE);
      if (retval != WAIT_OBJECT_0) {
        et.ShowLastErrMsg("WM_TIMER 01 WaitForSingleObject(MainReadEvent)");
      }
      if (degulgame.getGameState() == GameState::Playing ||
          degulgame.getGameState() == GameState::Lobby) {
        degulgame.update(hWnd);
      }
    }
    //WM_PAINT 호출
    InvalidateRect(hWnd, NULL, false);
    break;
    case 2:  // 좌클릭&홀드: power증가
      degulgame.increasePlayerPower();
      break;
    }
    break;
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    hBufferMemDC = CreateCompatibleDC(hDC);
    if (hBufferBitmap == NULL) {
      hBufferBitmap = CreateCompatibleBitmap(hDC, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    }
    oldBitmapBuffer = (HBITMAP)SelectObject(hBufferMemDC, hBufferBitmap);

    degulgame.draw(hDC, hBufferMemDC);

    /* 스타트 버튼 판정 범위
    if (degulgame.getGameState() == GameState::Lobby)
    {
        RECT temp;
        temp.left = 493; temp.right = 788; temp.top = 502; temp.bottom = 798;
        DrawFocusRect(hDC, &temp);
    }
    */

    SelectObject(hBufferMemDC, oldBitmapBuffer);
    DeleteDC(hBufferMemDC);
    EndPaint(hWnd, &ps);

    SetEvent(MainReadEvent);
    break;
  case WM_CREATE:
    SetTimer(hWnd, 1, 8, NULL);
    degulgame.SetConnector(&degul_connector);
    degulgame.initID();
    break;
  case WM_QUIT:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

DWORD __stdcall Messenger(LPVOID arg)
{
  static float eTime_Network; // elapsed Time between frame
  static clock_t lastClock; // elapsed Time between frame
  Connector* connector = (Connector*)arg;
  connector->SetGame(&degulgame);
  while (1) {
    eTime_Network = (float)(clock() - lastClock) / (float)CLOCKS_PER_SEC;
    lastClock = clock();

    std::cout << "\rNetwork eTime: " << eTime_Network << "ms, " << (int)(1.0f / eTime_Network) << "FPS      ";

    connector->ReceiveFromServer();
    DWORD retval = WaitForSingleObject(MainReadEvent, INFINITE);
    if (retval != WAIT_OBJECT_0) {
      et.ShowLastErrMsg("WaitForSingleObject(MainReadEvent)");
      break;
    }
    degulgame.UpdateFromBuf();
    degulgame.WriteDataToBuf();
    connector->SendToServer();
    SetEvent(MainReadEvent);
  }
  connector->CloseClient();
  return 0;
}

void showError(int code) {
  switch (code) {
  case MSG_ERR_GAME_ONPROGRESS:
    degulgame.SetErrorCode(-1);
    et.ShowMsg("Game is already on progress");
    break;
  default:
    break;
  }
}