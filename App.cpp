
#include <wrl.h>        // CommandLineToArgvW

#include "App.h"
#include "AppWindow.h"

HINSTANCE g_hInstance;
int g_nCmdShow;
HWND g_hWnd;

/*
Microsoft::WRL::ComPtr<ICoreWebView2> g_webview;
Microsoft::WRL::ComPtr<ICoreWebView2_3> g_webview3;
Microsoft::WRL::ComPtr<ICoreWebView2Controller> g_webViewController;
Microsoft::WRL::ComPtr<ICoreWebView2Environment> g_webViewEnvironment;

HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env);
HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
*/
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

	/*
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
		MessageBox(
			nullptr,
			_T("Call to RegisterClassEx failed!"),
			_T("WebView2Runner"),
			0);

	}

	g_hWnd =  CreateWindowEx(
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
		WS_EX_CLIENTEDGE,		// extended window style
		szWindowClass,			// pointer to registered class name
		szTitle,				// pointer to window name
		WS_OVERLAPPEDWINDOW,	//  window style
		CW_USEDEFAULT,			// horizontal position of window
		CW_USEDEFAULT,			// vertical position of window
		CW_USEDEFAULT,			//  window width
		CW_USEDEFAULT,			// window height
		nullptr,				// handle to parent or owner window
		nullptr,				// handle to menu, or child-window identifier
		g_hInstance,			// handle to application instance
		nullptr					// pointer to window-creation data
	);


	if (!g_hWnd)
	{
		MessageBox(
			nullptr,
			_T("Call to CreateWindow failed!"),
			_T("WebView2Runner"),
			0);
		return 0;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	CreateCoreWebView2EnvironmentWithOptions(
		nullptr,
		nullptr,
		nullptr,
		Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			OnCreateEnvironmentCompleted
		).Get());
		*/

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
/*
HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env)
{
	
	return env->CreateCoreWebView2Controller(g_hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
		OnCreateCoreWebView2ControllerCompleted
	).Get());
}

HRESULT OnCreateCoreWebView2ControllerCompleted(
	HRESULT result, ICoreWebView2Controller* controller)
{
	if (controller != nullptr) {
		g_webViewController = controller;
		g_webViewController->get_CoreWebView2(&g_webview);
	}

	Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
	g_webview->get_Settings(&settings);

#ifdef Debug
	settings->put_AreDevToolsEnabled(TRUE);
#endif

	RECT bounds;
	GetClientRect(g_hWnd, &bounds);
	g_webViewController->put_Bounds(bounds);

	g_webview->Navigate(L"https://google.com");

	return S_OK;
}*/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (auto app = (AppWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA)) {
		LRESULT result = 0;
		
		if (app->HandleWindowMessage(hWnd, message, wParam, lParam, &result)) {
			return result;
		}
	}

	switch (message)
	{/*
		case WM_SIZE:
			if (g_webViewController != nullptr) {
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				g_webViewController->put_Bounds(bounds);
				return 1;
			};
			break;*/
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}