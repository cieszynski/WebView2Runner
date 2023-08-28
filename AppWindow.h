#pragma once

#include "App.h"
#include "framework.h"

class AppWindow
{
public:
	AppWindow(
		HINSTANCE hInstance, int nCmdShow, AppEnv appEnv);// const std::wstring szURL, const std::wstring szUserDataFolder);

	bool HandleWindowMessage(
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

private:
	HWND m_hWnd;
	AppEnv m_appEnv;
	std::wstring m_szURL;
	std::wstring m_szUserDataFolder;

	Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
	Microsoft::WRL::ComPtr<ICoreWebView2_3> m_webview3;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_webViewController;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;

	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env);
	HRESULT OnCreateCoreWebView2ControllerCompleted(
		HRESULT result, ICoreWebView2Controller* controller);
	HRESULT OnAddScriptToExecuteOnDocumentCreatedCompleted(HRESULT errorCode, LPCWSTR id);
};

