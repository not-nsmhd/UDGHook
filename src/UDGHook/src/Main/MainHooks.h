#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Common/Types.h"

namespace UDGHook::Main
{
	struct
	{
		HWND Handle{};
	} WindowData;

	namespace FuncDefs
	{
		typedef LRESULT(*WINAPI WindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		typedef int (*GameInit)();
		typedef void (*TryGameInit)();

		typedef void (*RenderFrame)();
	}

	struct
	{
		FuncDefs::WindowProc WindowProc{};

		FuncDefs::GameInit GameInit{};
		FuncDefs::TryGameInit TryGameInit{};

		FuncDefs::RenderFrame RenderFrame{};
	} OGFuncs;

	namespace HookedFuncs
	{
		LRESULT WINAPI WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void TryGameInit();

		void RenderFrame();
	}

	bool InitHooks(offset_t baseAddress);
	void DestroyHooks();
}
