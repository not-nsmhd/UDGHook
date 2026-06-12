#include "GFXHooks.h"
#include <detours.h>

#include "ImGui/imgui.h"
#include "ImGui/Backends/imgui_impl_win32.h"
#include "ImGui/Backends/imgui_impl_dx11.h"

namespace UDGHook::GFX
{
	static bool Initialized{};
	static offset_t BaseAddress{};

	static constexpr const char LogName[] = "UDGHook::GFX";

	static OGFuncs ogFuncs{};
	static OGData ogData{};
	static D3DData d3d{};

	bool InitHooks(offset_t baseAddress)
	{
		BaseAddress = baseAddress;

		ogFuncs.UnbindVertexBuffer = reinterpret_cast<FuncDefs::UnbindVertexBuffer>(baseAddress + 0x129800);
		ogFuncs.ExecuteCommandBuffer = reinterpret_cast<FuncDefs::ExecuteCommandBuffer>(baseAddress + 0x124AA0);
		ogFuncs.RenderFrame = reinterpret_cast<FuncDefs::RenderFrame>(baseAddress + 0x121D10);
		
		ogFuncs.DrawCommandFuncs = reinterpret_cast<FuncDefs::DrawCommandFunc*>(baseAddress + 0x30AED0);

		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		//if (DetourAttach(&(PVOID&)ogFuncs.RenderFrame, HookedFuncs::RenderFrame) != NO_ERROR) { return false; }

		DetourTransactionCommit();

		LogInfo(LogName, "Hooked functions");
		return true;
	}

	void DestroyHooks()
	{
		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)ogFuncs.RenderFrame, HookedFuncs::RenderFrame);

		DetourTransactionCommit();
		LogInfo(LogName, "Detached hooked functions");
	}

	bool Init()
	{
		d3d.Device = *reinterpret_cast<ID3D11Device**>(BaseAddress + 0x82C190);
		d3d.DevContext = *reinterpret_cast<ID3D11DeviceContext**>(BaseAddress + 0x82C198);

		ogData.FirstDrawCommand = reinterpret_cast<DrawCommand*>(BaseAddress + 0x826490);
		ogData.MaxDrawCommands = reinterpret_cast<i16*>(BaseAddress + 0x802B72);

		ogData.LoadedTextures = reinterpret_cast<Texture*>(BaseAddress + 0x834E40);
		ogData.TextureExistence = reinterpret_cast<bool_t*>(BaseAddress + 0x8349F0);
		ogData.TextureNames = reinterpret_cast<char**>(BaseAddress + 0x076B750);

		LogInfo(LogName, "Max draw commands: %d", *ogData.MaxDrawCommands);
		LogInfo(LogName, "Initialized GFX");
		return true;
	}

	void Destroy()
	{
	}

	D3DData* GetD3DData()
	{
		return &d3d;
	}

	OGData* GetOGData()
	{
		return &ogData;
	}

	void HookedFuncs::RenderFrame()
	{
		ogFuncs.RenderFrame();
	}
}
