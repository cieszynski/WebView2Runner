#pragma once

#include "framework.h"
#include "resource.h"
#include "WebView2.h"

struct AppEnv {
	std::wstring szURL;
	std::wstring szMODE;
	std::wstring szUserDataFolder;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);