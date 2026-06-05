#include <Windows.h>
#include "Common.h"

namespace UDGHook
{
	void FatalError(const char* message)
	{
		MessageBoxA(NULL, "UDGHook - Error", message, MB_ICONERROR);
	}
}
