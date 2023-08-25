#include "AppWindow.h"

#include "App.h"

#define MAX_LOADSTRING 100

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

AppWindow::AppWindow()
{
	TCHAR szTitle[256];                  // Titelleistentext
	TCHAR szWindowClass[256];            // Der Klassenname des Hauptfensters.

	// https://learn.microsoft.com/en-us/windows/win32/winmsg/using-the-multiple-document-interface#creating-a-child-window
	LoadStringW(g_hInstance, IDS_APP_TITLE, szTitle, sizeof(szTitle) / sizeof(TCHAR));
	LoadStringW(g_hInstance, IDC_WEBVIEW2RUNNER, szWindowClass, sizeof(szWindowClass) / sizeof(TCHAR));

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = LoadIcon(g_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("WebView2Runner"),
			NULL);

	}

	HWND hWnd = CreateWindowEx(
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
		WS_EX_CLIENTEDGE,		// extended window style
		szWindowClass,			// pointer to registered class name
		szTitle,				// pointer to window name
		WS_OVERLAPPEDWINDOW,	//  window style
		CW_USEDEFAULT,			// horizontal position of window
		CW_USEDEFAULT,			// vertical position of window
		CW_USEDEFAULT,			//  window width
		CW_USEDEFAULT,			// window height
		NULL,					// handle to parent or owner window
		NULL,					// handle to menu, or child-window identifier
		g_hInstance,			// handle to application instance
		NULL					// pointer to window-creation data
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("WebView2Runner"),
			NULL);
	}
	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, g_nCmdShow);
	UpdateWindow(hWnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}