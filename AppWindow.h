#pragma once

#include "App.h"
#include "framework.h"
#include "Utils.h"

class AppWindow
{
public:
	AppWindow(
		HINSTANCE hInstance, int nCmdShow, AppEnv appEnv);
	void SetFullscreen(BOOL boolValue);
	bool HandleWindowMessage(
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

private:
	HWND m_hWnd;
	AppEnv m_appEnv;
	std::wstring m_szURL;
	std::wstring m_szUserDataFolder;

	Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
	Microsoft::WRL::ComPtr<ICoreWebView2_3> m_webView3;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_webViewController;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;

	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env);
	HRESULT OnCreateCoreWebView2ControllerCompleted(
		HRESULT result, ICoreWebView2Controller* controller);
	HRESULT OnAddScriptToExecuteOnDocumentCreatedCompleted(HRESULT errorCode, LPCWSTR id);
};

