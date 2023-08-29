#include "AppWindow.h"
#include "App.h"

#include <WebView2EnvironmentOptions.h>
#include "HostObject.h"

AppWindow::AppWindow(
	HINSTANCE hInstance, 
	int nCmdShow, 
	AppEnv appEnv
)
{
	TCHAR szTitle[256];                  // Titelleistentext
	TCHAR szWindowClass[256];            // Der Klassenname des Hauptfensters.

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, sizeof(szTitle) / sizeof(TCHAR));
	LoadStringW(hInstance, IDC_WEBVIEW2RUNNER, szWindowClass, sizeof(szWindowClass) / sizeof(TCHAR));

	m_appEnv = appEnv;

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


	// WindowStyles:
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = NULL;// WS_EX_CLIENTEDGE;
	int w = CW_USEDEFAULT;
	int h = CW_USEDEFAULT;

	if (std::wstring::npos != m_appEnv.szMODE.find(L"kiosk")) {
		dwExStyle = WS_EX_TOPMOST;
	}

	if ((std::wstring::npos != m_appEnv.szMODE.find(L"fullscreen"))
		|| (std::wstring::npos != m_appEnv.szMODE.find(L"kiosk"))) {
		dwStyle = WS_POPUP;
		w = GetSystemMetrics(SM_CXSCREEN);
		h = GetSystemMetrics(SM_CYSCREEN);
	}

	m_hWnd = CreateWindowEx(
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
		dwExStyle,				// extended window style
		szWindowClass,			// pointer to registered class name
		szTitle,				// pointer to window name
		dwStyle,				// window style
		CW_USEDEFAULT,			// horizontal position of window
		CW_USEDEFAULT,			// vertical position of window
		w,						// window width
		h,						// window height
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

		auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
		options->put_AdditionalBrowserArguments(L"--enable-local-file-accesses");

		CreateCoreWebView2EnvironmentWithOptions(
			nullptr,
			m_appEnv.szUserDataFolder.c_str(),
			options.Get(),
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
		m_webViewController->get_CoreWebView2(&m_webView);
	}

	Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
	m_webView->get_Settings(&settings);

#ifdef _DEBUG
	settings->put_AreDevToolsEnabled(TRUE);
	settings->put_AreDefaultScriptDialogsEnabled(TRUE);
#else
	settings->put_AreDefaultScriptDialogsEnabled(FALSE);
	settings->put_AreDevToolsEnabled(FALSE);
#endif
	settings->put_AreDefaultContextMenusEnabled(FALSE);
	settings->put_AreHostObjectsAllowed(TRUE);
		
	Microsoft::WRL::ComPtr<ICoreWebView2_3> webview3;
	if (SUCCEEDED(m_webView->QueryInterface<ICoreWebView2_3>(&webview3)))
	{
		webview3->SetVirtualHostNameToFolderMapping(L"assets", L"assets",
			COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS);
	}

	Microsoft::WRL::ComPtr<HostObject> m_hostObject;
	m_hostObject = Microsoft::WRL::Make <HostObject>(this);

	VARIANT remoteObjectAsVariant = {};
	m_hostObject->QueryInterface(IID_PPV_ARGS(&remoteObjectAsVariant.pdispVal));
	remoteObjectAsVariant.vt = VT_DISPATCH;
	m_webView->AddHostObjectToScript(L"runner", &remoteObjectAsVariant);
	remoteObjectAsVariant.pdispVal->Release();

	RECT bounds;
	GetClientRect(m_hWnd, &bounds);
	m_webViewController->put_Bounds(bounds);

	EventRegistrationToken token;
	m_webView->add_DocumentTitleChanged(
		Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
			[this](ICoreWebView2* webview, IUnknown* args) -> HRESULT {
				LPWSTR title;
				webview->get_DocumentTitle(&title);
				SetWindowTextW(m_hWnd, title);
				return S_OK;
			}).Get(), &token);

	m_webView->add_WindowCloseRequested(
		Microsoft::WRL::Callback<ICoreWebView2WindowCloseRequestedEventHandler>(
			[this](ICoreWebView2* webview, IUnknown* args) {
				DestroyWindow(m_hWnd);
				return S_OK;
			}).Get(), &token);

	m_webView->add_ContainsFullScreenElementChanged(
		Microsoft::WRL::Callback <ICoreWebView2ContainsFullScreenElementChangedEventHandler>(
			[this](ICoreWebView2* webview, IUnknown* args) {
				BOOL containsFullScreenElement;
				webview->get_ContainsFullScreenElement(&containsFullScreenElement);
				SetFullscreen(containsFullScreenElement);

				return S_OK;
			}).Get(), &token);

	std::wstring script = Utils::LoadStringFromResource(NULL, IDS_APP_SCRIPT);

	return m_webView->AddScriptToExecuteOnDocumentCreated(script.c_str(),
		Microsoft::WRL::Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
			this, &AppWindow::OnAddScriptToExecuteOnDocumentCreatedCompleted
	).Get());	
}

HRESULT AppWindow::OnAddScriptToExecuteOnDocumentCreatedCompleted(HRESULT errorCode, LPCWSTR id)
{
#ifdef _DEBUG
	m_webView->OpenDevToolsWindow();
#endif
	return m_webView->Navigate(m_appEnv.szURL.c_str());
}

void AppWindow::SetFullscreen(BOOL boolValue)
{
	// In kiosk mode toggle fullscreen is not allowed
	if (std::wstring::npos != m_appEnv.szMODE.find(L"kiosk")) {
		return;
	}

	if (!boolValue)
	{
		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, NULL, 0, 0, 600, 400, SWP_FRAMECHANGED);
	}
	else
	{
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
	}
}

bool AppWindow::HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	switch (message)
	{
		case WM_KEYDOWN:
			/*
			if (wParam == VK_F11) {
				SetFullscreen(GetWindowLongPtr(hWnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW);

				return true;
			}
			*/
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
