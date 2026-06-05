#include "MainHooks.h"
#include <detours.h>

#include "GFX/GFXHooks.h"
#include "Common/Logging/Logging.h"

#include "ImGui/imgui.h"
#include "ImGui/Backends/imgui_impl_win32.h"
#include "ImGui/Backends/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace UDGHook::Main
{
	static bool Initialized{};
	static offset_t BaseAddress{};

	static constexpr const char LogName[] = "UDGHook::Main";

	bool InitHooks(offset_t baseAddress)
	{
		BaseAddress = baseAddress;

		OGFuncs.WindowProc = reinterpret_cast<FuncDefs::WindowProc>(baseAddress + 0xF6320);

		OGFuncs.GameInit = reinterpret_cast<FuncDefs::GameInit>(baseAddress + 0x85850);
		OGFuncs.TryGameInit = reinterpret_cast<FuncDefs::TryGameInit>(baseAddress + 0x1052A0);

		OGFuncs.RenderFrame = reinterpret_cast<FuncDefs::RenderFrame>(baseAddress + 0x121D10);

		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (DetourAttach(&(PVOID&)OGFuncs.TryGameInit, HookedFuncs::TryGameInit) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.WindowProc, HookedFuncs::WindowProc) != NO_ERROR) { return false; }
		if (DetourAttach(&(PVOID&)OGFuncs.RenderFrame, HookedFuncs::RenderFrame) != NO_ERROR) { return false; }

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

		GFX::Init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		GFX::D3DData* d3dData = GFX::GetD3DData();

		ImGui_ImplWin32_Init(WindowData.Handle);
		ImGui_ImplDX11_Init(d3dData->Device, d3dData->DevContext);
		LogInfo(LogName, "ImGui initialized");
	}

	void HookedFuncs::RenderFrame()
	{
		OGFuncs.RenderFrame();

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("UDGHook - Testing");
		{
			ImGui::Text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
			ImGui::Text("c++ header inclusion specifics strike again (struct redefinition moment)");
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
