#include "MainHooks.h"
#include <detours.h>

#include "Common/Logging/Logging.h"

namespace UDGHook::Main
{
	static bool Initialized{};
	static offset_t BaseAddress{};

	static constexpr const char LogName[] = "UDGHook::Main";

	bool InitHooks(offset_t baseAddress)
	{
		BaseAddress = baseAddress;

		WindowData.WindowHandle = reinterpret_cast<HWND*>(baseAddress + 0x8264C0);

		OGFuncs.WindowProc = reinterpret_cast<FuncDefs::WindowProc>(baseAddress + 0xF6320);

		OGFuncs.GameInit = reinterpret_cast<FuncDefs::GameInit>(baseAddress + 0x85850);
		OGFuncs.TryGameInit = reinterpret_cast<FuncDefs::TryGameInit>(baseAddress + 0x1052A0);

		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (DetourAttach(&(PVOID&)OGFuncs.TryGameInit, HookedFuncs::TryGameInit) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.WindowProc, HookedFuncs::WindowProc) != NO_ERROR) { return false; }

		DetourTransactionCommit();

		LogInfo(LogName, "Hooked functions");
		return true;
	}

	void DestroyHooks()
	{
		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)OGFuncs.TryGameInit, HookedFuncs::TryGameInit);
		DetourDetach(&(PVOID&)OGFuncs.WindowProc, HookedFuncs::WindowProc);

		DetourTransactionCommit();
		LogInfo(LogName, "Detached hooked functions");
	}

	LRESULT WINAPI HookedFuncs::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return OGFuncs.WindowProc(hwnd, msg, wParam, lParam);
	}

	void HookedFuncs::TryGameInit()
	{
		int result = OGFuncs.GameInit();
	}
}
