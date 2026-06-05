#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Common/Types.h"
#include "Common/Logging/Logging.h"
#include "DrawCommands.h"

namespace UDGHook::GFX
{
	struct
	{
		ID3D11Device* Device{};
		ID3D11DeviceContext* DevContext{};
	} D3D;

	namespace FuncDefs
	{
		typedef void (*UnbindVertexBuffer)();
		typedef void (*ExecuteCommandBuffer)();

		typedef void (*DrawCommandFunc)(void* commandData);

		typedef void (*RenderFrame)();
	}

	struct
	{
		FuncDefs::UnbindVertexBuffer UnbindVertexBuffer{};
		FuncDefs::ExecuteCommandBuffer ExecuteCommandBuffer{};
		FuncDefs::DrawCommandFunc* DrawCommandFuncs{};
		FuncDefs::RenderFrame RenderFrame{};
	} OGFuncs;

	struct
	{
		DrawCommand* FirstDrawCommand{};
		i16* MaxDrawCommands{};
	} OGData;

	namespace HookedFuncs
	{
		void RenderFrame();
	}

	bool InitHooks(offset_t baseAddress);
	void DestroyHooks();

	bool Init();
	void Destroy();
}
