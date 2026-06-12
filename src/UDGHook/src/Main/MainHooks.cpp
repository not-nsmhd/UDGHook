#include "MainHooks.h"
#include <detours.h>

#include "GFX/GFXHooks.h"
#include "Common/Logging/Logging.h"

#include "ImGui/imgui.h"
#include "ImGui/Backends/imgui_impl_win32.h"
#include "ImGui/Backends/imgui_impl_dx11.h"

#include "MainMenu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace UDGHook::Main
{
	static bool Initialized{};
	static offset_t BaseAddress{};
	static bool ImGuiActive{};

	static constexpr const char LogName[] = "UDGHook::Main";

	bool InitHooks(offset_t baseAddress)
	{
		BaseAddress = baseAddress;

		OGFuncs.WindowProc = reinterpret_cast<FuncDefs::WindowProc>(baseAddress + 0xF6320);

		OGFuncs.GameInit = reinterpret_cast<FuncDefs::GameInit>(baseAddress + 0x85850);
		OGFuncs.TryGameInit = reinterpret_cast<FuncDefs::TryGameInit>(baseAddress + 0x1052A0);

		OGFuncs.GameQuit = reinterpret_cast<FuncDefs::GameQuit>(baseAddress + 0x85340);

		OGFuncs.UpdatePlayerInputs = reinterpret_cast<FuncDefs::UpdatePlayerInputs>(baseAddress + 0x105AD0);
		OGFuncs.RenderFrame = reinterpret_cast<FuncDefs::RenderFrame>(baseAddress + 0x121D10);

		OGFuncs.IsFocused = reinterpret_cast<FuncDefs::IsFocused>(baseAddress + 0x113380);

		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (DetourAttach(&(PVOID&)OGFuncs.TryGameInit, HookedFuncs::TryGameInit) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.GameQuit, HookedFuncs::GameQuit) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.WindowProc, HookedFuncs::WindowProc) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.UpdatePlayerInputs, HookedFuncs::UpdatePlayerInputs) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.RenderFrame, HookedFuncs::RenderFrame) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.IsFocused, HookedFuncs::IsFocused) != NO_ERROR) { return false; }

		DetourTransactionCommit();

		LogInfo(LogName, "Hooked functions");

		GFX::InitHooks(baseAddress);

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
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) { return TRUE; }
		return OGFuncs.WindowProc(hwnd, msg, wParam, lParam);
	}

	void HookedFuncs::TryGameInit()
	{
		OGFuncs.GameInit();
		WindowData.Handle = *reinterpret_cast<HWND*>(BaseAddress + 0x8264C0);
		WindowData.IsFocused = reinterpret_cast<bool*>(BaseAddress + 0x806061);

		GFX::Init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		GFX::D3DData* d3dData = GFX::GetD3DData();

		ImGui_ImplWin32_Init(WindowData.Handle);
		ImGui_ImplDX11_Init(d3dData->Device, d3dData->DevContext);
		LogInfo(LogName, "ImGui initialized");

		MainMenu::CreateInstance();
	}

	void HookedFuncs::GameQuit()
	{
		MainMenu::DestoryInstance();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		LogInfo(LogName, "ImGui destroyed");

		OGFuncs.GameQuit();
	}

	void HookedFuncs::UpdatePlayerInputs(int unknown)
	{
		OGFuncs.UpdatePlayerInputs(unknown);

		auto& io = ImGui::GetIO();
		if (ImGui::IsKeyPressed(ImGuiKey_Insert, false))
		{ 
			ImGuiActive = !ImGuiActive; 
			MainMenu::GetInstance()->Toggle();
		}
	}

	void HookedFuncs::RenderFrame()
	{
		OGFuncs.RenderFrame();

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		MainMenu::GetInstance()->OnRender();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	bool HookedFuncs::IsFocused()
	{
		if (ImGuiActive && MainMenu::GetInstance()->ShouldInputsBeDisabled()) { return false; }
		return *WindowData.IsFocused;
	}

	bool IsImGuiActive()
	{
		return ImGuiActive;
	}
}
