#include "ErrorTeller.h"
#include <iostream>
using namespace std;

DWORD WINAPI CreateMessageBox(LPVOID lpParam) {
  size_t s;
  wchar_t wMsg[256];
  setlocale(LC_ALL, "");
  mbstowcs_s(&s, wMsg, (char*)lpParam, strlen((char*)lpParam) + 1);
  MessageBox(NULL, wMsg, L"Error", MB_ICONERROR | MB_TOPMOST);
  return 0;
}

ErrorTeller::ErrorTeller()
{
    this->hWnd = NULL;
}

ErrorTeller::ErrorTeller(HWND hWnd)
{
    this->hWnd = hWnd;
}

void ErrorTeller::ShowLastErrMsg(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    cout << "Error)" << msg << ", " << (char*)lpMsgBuf << endl;

    size_t s;
    wchar_t wMsg[256];
    wchar_t wTitle[256];
    setlocale(LC_ALL, "");
    mbstowcs_s(&s, wMsg, (char*)lpMsgBuf, strlen((char*)lpMsgBuf) + 1);
    mbstowcs_s(&s, wTitle, msg, strlen(msg));
    MessageBox(hWnd, wMsg, wTitle, MB_ICONERROR | MB_TOPMOST);

    LocalFree(lpMsgBuf);
}

void ErrorTeller::ShowMsg(const char* msg)
{
    CreateThread(NULL, 0, &CreateMessageBox, (LPVOID)msg, 0, NULL);
    cout << "Error)" << msg << endl;
}
