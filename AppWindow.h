#pragma once

#include "framework.h"

class AppWindow
{
public:
	AppWindow(HINSTANCE hInstance, int nCmdShow);

	bool HandleWindowMessage(
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

private:
	HWND m_hWnd;

	Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
	Microsoft::WRL::ComPtr<ICoreWebView2_3> m_webview3;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_webViewController;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;

	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env);
	HRESULT OnCreateCoreWebView2ControllerCompleted(
		HRESULT result, ICoreWebView2Controller* controller);
};

