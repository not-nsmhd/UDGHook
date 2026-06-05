#include "Common/Common.h"
#include "Main/MainHooks.h"

static bool InitConsoleOutput()
{
	// Copied and modified from https://github.com/morgana-x/dr_menu/blob/master/dr_menu/Core.cpp
	if (!AllocConsole()) { return false; }

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	return true;
}

static void CloseConsoleOutput()
{
	FreeConsole();
}

bool InitHooks()
{
	offset_t baseAddr = (offset_t)(GetModuleHandle(NULL));

#ifdef _DEBUG
	if (InitConsoleOutput() == 0)
	{
		UDGHook::FatalError("Failed to allocate console");
		return false;
	}
#endif // _DEBUG

	UDGHook::Main::InitHooks(baseAddr);

	return true;
}

void DestroyHooks()
{
	UDGHook::Main::DestroyHooks();

#ifdef _DEBUG
	CloseConsoleOutput();
#endif // _DEBUG
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		if (!InitHooks())
		{
			return FALSE;
		}
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		DestroyHooks();
	}
	return TRUE;
}
