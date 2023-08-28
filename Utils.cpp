#include "Utils.h"



std::wstring Utils::LoadStringFromResource(HINSTANCE instance, UINT stringID)
{
    WCHAR* pBuf = nullptr;
    int len = LoadStringW(
        instance,
        stringID,
        reinterpret_cast<LPWSTR>(&pBuf),
        0);

    return std::wstring(pBuf, len);
}
