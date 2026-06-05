#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Common/Types.h"
#include "Common/Logging/Logging.h"
#include "DrawCommands.h"

namespace UDGHook::GFX
{
	namespace FuncDefs
	{
		typedef void (*UnbindVertexBuffer)();
		typedef void (*ExecuteCommandBuffer)();

		typedef void (*DrawCommandFunc)(void* commandData);

		typedef void (*RenderFrame)();
	}

	struct OGFuncs
	{
		FuncDefs::UnbindVertexBuffer UnbindVertexBuffer{};
		FuncDefs::ExecuteCommandBuffer ExecuteCommandBuffer{};
		FuncDefs::DrawCommandFunc* DrawCommandFuncs{};
		FuncDefs::RenderFrame RenderFrame{};
	};

	struct D3DData
	{
		ID3D11Device* Device{};
		ID3D11DeviceContext* DevContext{};
	};

	struct OGData
	{
		DrawCommand* FirstDrawCommand{};
		i16* MaxDrawCommands{};
	};

	namespace HookedFuncs
	{
		void RenderFrame();
	}

	bool InitHooks(offset_t baseAddress);
	void DestroyHooks();

	bool Init();
	void Destroy();

	D3DData* GetD3DData();
}
