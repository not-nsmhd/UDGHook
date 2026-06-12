#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Common/Types.h"

namespace UDGHook::Main
{
	struct
	{
		HWND Handle{};
		bool* IsFocused{};
	} WindowData;

	namespace FuncDefs
	{
		typedef LRESULT(*WINAPI WindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		typedef int (*GameInit)();
		typedef void (*TryGameInit)();

		typedef void (*GameQuit)();

		typedef void (*UpdatePlayerInputs)(int unknown);
		typedef void (*RenderFrame)();

		typedef bool (*IsFocused)();
	}

	struct
	{
		FuncDefs::WindowProc WindowProc{};

		FuncDefs::GameInit GameInit{};
		FuncDefs::TryGameInit TryGameInit{};

		FuncDefs::GameQuit GameQuit{};

		FuncDefs::UpdatePlayerInputs UpdatePlayerInputs{};
		FuncDefs::RenderFrame RenderFrame{};

		FuncDefs::IsFocused IsFocused{};
	} OGFuncs;

	namespace HookedFuncs
	{
		LRESULT WINAPI WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void TryGameInit();

		void GameQuit();

		void UpdatePlayerInputs(int unknown);
		void RenderFrame();

		bool IsFocused();
	}

	bool InitHooks(offset_t baseAddress);
	void DestroyHooks();

	bool IsImGuiActive();
}
