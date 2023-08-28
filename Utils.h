#pragma once

#include <windows.h>
#include <string>
#include <tchar.h>

class Utils
{
public:
	static std::wstring LoadStringFromResource(HINSTANCE instance, UINT stringID);
};

