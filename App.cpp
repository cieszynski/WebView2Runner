
#include <wrl.h>        // CommandLineToArgvW

#include "App.h"
#include "AppWindow.h"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    LPWSTR* szArglist;
    int i, nArgs;

    // https://learn.microsoft.com/de-de/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (NULL == szArglist)
    {
        MessageBox(nullptr, _T("CommandLineToArgvW failed\n"), _T("Error"), 0);
        return 0;
    }

    for (i = 1; i < nArgs; i++) {

    }

    LocalFree(szArglist);

	new AppWindow(hInstance, nCmdShow);


    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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