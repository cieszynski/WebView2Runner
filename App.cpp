
#include "App.h"
#include "AppWindow.h"

HINSTANCE g_hInstance;
int g_nCmdShow;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    g_hInstance = hInstance;
    g_nCmdShow = nCmdShow;

    new AppWindow();

    MSG msg;

    // Hauptnachrichtenschleife:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}