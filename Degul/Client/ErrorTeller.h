#pragma warning(disable:4819)
#pragma once
#include <Windows.h>

class ErrorTeller {
public:
	ErrorTeller();
	ErrorTeller(HWND);
	void ShowLastErrMsg(const char*);
	void ShowMsg(const char*);

	void SetHwnd(HWND hWnd) { this->hWnd = hWnd; }

private:
	HWND hWnd;
};