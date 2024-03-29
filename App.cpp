
#include <wrl.h>        // CommandLineToArgvW
#include "App.h"
#include "AppWindow.h"
#include "Utils.h"
#include <filesystem>	// std::filesystem c++17

HHOOK hKeyboardHook;

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	AppEnv appEnv;

	appEnv.szURL = Utils::LoadStringFromResource(hInstance, IDS_APP_URL);
	appEnv.szMODE = Utils::LoadStringFromResource(hInstance, IDS_APP_MODE);
	appEnv.szUserDataFolder = std::filesystem::temp_directory_path().wstring();

	if (lpCmdLine && lpCmdLine[0]) {
		int i, nArgs = 0;
		LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		for (i = 1; i < nArgs; i++) {
			if ((0 == lstrcmpW(szArglist[i], L"--url")) && (++i < nArgs)) {
				appEnv.szURL = szArglist[i];
			}

			if ((0 == lstrcmpW(szArglist[i], L"--udf")) && (++i < nArgs)) {
				appEnv.szUserDataFolder = szArglist[i];
			}

			if ((0 == lstrcmpW(szArglist[i], L"--mode")) && (++i < nArgs)) {
				appEnv.szMODE = szArglist[i];
			}

			// if ((0 == lstrcmpW(szArglist[i], L"--options")) && (++i < nArgs)) {
			// 	szUserDataFolder = szArglist[i];
			// }
		}

		LocalFree(szArglist);
	}


	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);

	new AppWindow(hInstance, nCmdShow, appEnv);


    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	UnhookWindowsHookEx(hKeyboardHook);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (auto app = (AppWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA)) {
		LRESULT result = 0;
		
		if (app->HandleWindowMessage(hWnd, message, wParam, lParam, &result)) {
			return result;
		}
	}

	switch (message)
	{
	    case WM_CLOSE:
		    if (MessageBox(hWnd, L"Programm wirklich beenden?", L"Frage:", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		    {
			    DestroyWindow(hWnd);
		    }
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// keyboard hook proc
// prevent screenshots like ALT+PRINT/WINKEY+PRINT/WINKEY+SHIFT+S
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

	if (p->vkCode == VK_RWIN || p->vkCode == VK_LWIN) return 1; // disable windows key
	if (p->vkCode == VK_PRINT || p->vkCode == VK_SNAPSHOT) return 1; // disable print/snapshot
	
	CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}