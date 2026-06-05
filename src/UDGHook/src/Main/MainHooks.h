#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Common/Types.h"

namespace UDGHook::Main
{
	struct
	{
		HWND* WindowHandle{};
	} WindowData;

	namespace FuncDefs
	{
		typedef LRESULT(*WINAPI WindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		//typedef void (*FatalError)(const char* format, ...);

		typedef int (*GameInit)();
		typedef void (*TryGameInit)();

		//typedef void (*UpdatePlayerInputs)(int unknown);
		//typedef void (*Log_InGame)(const char* format, ...);
	}

	struct
	{
		FuncDefs::WindowProc WindowProc{};

		FuncDefs::GameInit GameInit{};
		FuncDefs::TryGameInit TryGameInit{};

		//FuncDefs::FatalError FatalError{};
		//FuncDefs::UpdatePlayerInputs UpdatePlayerInputs{};
		//FuncDefs::Log_InGame Log_InGame{};
	} OGFuncs;

	namespace HookedFuncs
	{
		LRESULT WINAPI WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void TryGameInit();
	}

	bool InitHooks(offset_t baseAddress);
	void DestroyHooks();
}
