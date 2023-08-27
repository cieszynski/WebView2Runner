#include "AppWindow.h"
#include "App.h"

//#include <WebView2EnvironmentOptions.h>
#include "HostObject.h"

AppWindow::AppWindow(HINSTANCE hInstance, int nCmdShow) {

	TCHAR szTitle[256];                  // Titelleistentext
	TCHAR szWindowClass[256];            // Der Klassenname des Hauptfensters.

	// https://learn.microsoft.com/en-us/windows/win32/winmsg/using-the-multiple-document-interface#creating-a-child-window
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, sizeof(szTitle) / sizeof(TCHAR));
	LoadStringW(hInstance, IDC_WEBVIEW2RUNNER, szWindowClass, sizeof(szWindowClass) / sizeof(TCHAR));

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);


	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			MB_ICONERROR);
	}

	m_hWnd = CreateWindowEx(
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
		hInstance,				// handle to application instance
		nullptr					// pointer to window-creation data
	);

	if (!m_hWnd)
	{
		MessageBox(
			nullptr,
			_T("Call to CreateWindow failed!"),
			_T("WebView2Runner"),
			MB_ICONERROR);
	}
	else
	{
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

		ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);

		CreateCoreWebView2EnvironmentWithOptions(
			nullptr,
			nullptr,
			nullptr,
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				this, &AppWindow::OnCreateEnvironmentCompleted
			).Get());
	}
}

HRESULT AppWindow::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env)
{

	return env->CreateCoreWebView2Controller(m_hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
		this, &AppWindow::OnCreateCoreWebView2ControllerCompleted
	).Get());
}

HRESULT AppWindow::OnCreateCoreWebView2ControllerCompleted(
	HRESULT result, ICoreWebView2Controller* controller)
{
	if (controller != nullptr) {
		m_webViewController = controller;
		m_webViewController->get_CoreWebView2(&m_webview);
	}

	Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
	m_webview->get_Settings(&settings);

#ifdef Debug
	settings->put_AreDevToolsEnabled(TRUE);
#endif
	settings->put_AreDefaultContextMenusEnabled(FALSE);
	settings->put_AreHostObjectsAllowed(TRUE);
	//settings->put_AreDefaultScriptDialogsEnabled(FALSE);
	
	Microsoft::WRL::ComPtr<ICoreWebView2_3> webview3;
	if (!m_webview->QueryInterface<ICoreWebView2_3>(&webview3))
	{
		webview3->SetVirtualHostNameToFolderMapping(L"assets", L"assets",
			COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS);
	}

	Microsoft::WRL::ComPtr<HostObject> m_hostObject;
	m_hostObject = Microsoft::WRL::Make <HostObject>(
		[](std::function<void(void)> callback) {}
	);
	m_hostObject->set_AppWindow(this);

	VARIANT remoteObjectAsVariant = {};
	m_hostObject->QueryInterface(IID_PPV_ARGS(&remoteObjectAsVariant.pdispVal));
	remoteObjectAsVariant.vt = VT_DISPATCH;
	m_webview->AddHostObjectToScript(L"runner", &remoteObjectAsVariant);
	remoteObjectAsVariant.pdispVal->Release();


	RECT bounds;
	GetClientRect(m_hWnd, &bounds);
	m_webViewController->put_Bounds(bounds);

	//m_webview->Navigate(L"https://google.com");
	webview3->Navigate(L"https://assets/index.html");
	return S_OK;
}

bool AppWindow::HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	switch (message)
	{
		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				//Toggle fullscreen
				if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP)
				{
					SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
					SetWindowPos(hWnd, NULL, 0, 0, 600, 400, SWP_FRAMECHANGED);
				}
				else
				{
					int w = GetSystemMetrics(SM_CXSCREEN);
					int h = GetSystemMetrics(SM_CYSCREEN);
					SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
					SetWindowPos(hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
				}
				return true;
			}
			break;
		case WM_SIZE:
			if (m_webViewController != nullptr) {
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				m_webViewController->put_Bounds(bounds);
				return true;
			};
			break;
	}
    return false;
}
