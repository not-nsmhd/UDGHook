#include <Windows.h>
#include <d3d11.h>
#include <detours.h>
#include <array>
#include <string_view>

#include "ImGui/imgui.h"
#include "ImGui/Backends/imgui_impl_win32.h"
#include "ImGui/Backends/imgui_impl_dx11.h"

#include "Common/Types.h"
#include "Common/Logging/Logging.h"

using namespace UDGHook;

static offset_t baseAddr{};
static bool ImGuiActive = false;
static float TotalElapsedTime = 0.0f;

static int* DebugTextLength{};
static char* DebugTextBuffer{};

namespace Gfx
{
	static ID3D11Device** d3dDevice{};
	static ID3D11DeviceContext** d3dDevContext{};

	enum class DrawCommandType : i64
	{
		None,

		Unknown_0x1,
		Unknown_0x2,

		Model,

		Unknown_0x4,

		SingleSprite,
		MultipleSprites,

		Unknown_0x7,

		SingleQuad,
		MultipleQuads,
		SingleLine,
		MultipleLines,
		SetScissorRect,

		Count
	};

	static constexpr std::array<std::string_view, UDGHook::EnumCount<DrawCommandType>()> DrawCommandTypeNames
	{
		"None",
		"Unknown (0x1)",
		"Unknown (0x2)",

		"3D Model",

		"Unknown (0x4)",

		"Single Sprite",
		"Multiple Sprites",

		"Unknown (0x7)",

		"Single Quad",
		"Multiple Quads",

		"Single Line",
		"Multiple Lines",

		"Set Scissor Rectangle",
	};

	struct DrawCommand_Base
	{
		DrawCommand_Base* Next{};
		DrawCommandType Type{};
		void* Data{}; // Specific to each command type
	};

	struct DrawCommand_SingleSpriteData
	{
		i16 TexPage{};
		i16 Flags{};
		int SpriteOffset{};
	};

	struct DrawCommand_MultipleSpritesData
	{
		int SpriteCount{};
		i16 TexPage{};
		i16 Flags{};
		int SpriteOffset{};
	};

	namespace FuncDefs
	{
		typedef void (*UnbindVertexBuffer)();
		typedef void (*ExecuteCommandBuffer)();

		typedef void (*DrawCommandFunc)(void* commandData);

		typedef void (*RenderFrame)();
	}

	DrawCommand_Base* FirstDrawCommand{};
	i16* MaxDrawCommands{};

	FuncDefs::UnbindVertexBuffer UnbindVertexBuffer{};
	FuncDefs::ExecuteCommandBuffer ExecuteCommandBuffer{};

	FuncDefs::DrawCommandFunc* DrawCommandFuncs{};

	FuncDefs::RenderFrame RenderFrame{};

	void ConstructDrawCommandInfo_ImGui(const DrawCommand_Base* command)
	{
		if (command == nullptr) { return; }
		switch (command->Type)
		{
		case DrawCommandType::SingleSprite:
		{
			const DrawCommand_SingleSpriteData* cmdData = reinterpret_cast<const DrawCommand_SingleSpriteData*>(&command->Data);

			ImGui::Text("Texture Page: %d", cmdData->TexPage);
			ImGui::Text("Flags: 0x%X", cmdData->Flags);
			ImGui::Text("Sprite Offset: %d", cmdData->SpriteOffset);

			break;
		}
		case DrawCommandType::MultipleSprites:
		{
			const DrawCommand_MultipleSpritesData* cmdData = reinterpret_cast<const DrawCommand_MultipleSpritesData*>(&command->Data);

			ImGui::Text("Texture Page: %d", cmdData->TexPage);
			ImGui::Text("Flags: 0x%X", cmdData->Flags);
			ImGui::Text("Sprite Offset: %d", cmdData->SpriteOffset);
			ImGui::Text("Sprite Count: %d", cmdData->SpriteCount);

			break;
		}
		}
	}

	namespace HookedFuncs
	{
		void ExecuteCommandBuffer()
		{
			/*UnbindVertexBuffer();

			DrawCommand_Base* firstCommand = FirstDrawCommand;
			DrawCommand_Base* cmd = FirstDrawCommand;

			size_t totalCommands = 0;

			if (FirstDrawCommand != nullptr)
			{
				do
				{
					if (cmd->Type < DrawCommandType::Count && cmd->Type > DrawCommandType::None)
					{
						size_t cmdFuncIndex = static_cast<size_t>(cmd->Type) - 1;
						DrawCommandFuncs[cmdFuncIndex](&cmd->Data);

						totalCommands++;
					}
					cmd = cmd->Next;
				} while (cmd != (firstCommand + 16 * static_cast<size_t>(*MaxDrawCommands)) && (cmd != nullptr));
			}*/

			Gfx::ExecuteCommandBuffer();
		}

		void RenderFrame()
		{
			Gfx::RenderFrame();

			/*static constexpr const char debugMessage[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
			strncpy_s(DebugTextBuffer, 2048, debugMessage, sizeof(debugMessage));
			*DebugTextLength = sizeof(debugMessage);*/

			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX11_NewFrame();
			ImGui::NewFrame();

			ImGuiIO& io = ImGui::GetIO();
			TotalElapsedTime += io.DeltaTime;

			if (TotalElapsedTime < 5.0f)
			{
				ImGuiWindowFlags flags = 0;
				flags |= ImGuiWindowFlags_NoMove;
				flags |= ImGuiWindowFlags_NoResize;
				flags |= ImGuiWindowFlags_NoTitleBar;
				flags |= ImGuiWindowFlags_NoCollapse;
				flags |= ImGuiWindowFlags_NoInputs;
				flags |= ImGuiWindowFlags_AlwaysAutoResize;
				ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
				if (ImGui::Begin("##udghook-dispnotice", nullptr, flags))
				{
					ImGui::Text("Press the Insert key to display UDGHook's test window.");
					ImGui::End();
				}
			}

			if (ImGuiActive)
			{
				DrawCommand_Base* cmd = FirstDrawCommand;
				int commands = 0;

				ImGui::Begin("UDGHook - Testing");
				{
					ImGui::Text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
					if (ImGui::TreeNode("Draw Command Buffer"))
					{
						if (FirstDrawCommand != nullptr)
						{
							do
							{
								if (cmd->Type < DrawCommandType::Count && cmd->Type > DrawCommandType::None)
								{
									ImGui::PushID(commands);

									const std::string_view& typeName = DrawCommandTypeNames[static_cast<size_t>(cmd->Type)];
									if (ImGui::TreeNode("", "Command %02d (%s)", commands, typeName.data()))
									{
										Gfx::ConstructDrawCommandInfo_ImGui(cmd);
										ImGui::TreePop();
									}

									commands++;
									ImGui::PopID();
								}
								cmd = cmd->Next;
							} while (cmd != (FirstDrawCommand + 16 * static_cast<size_t>(*MaxDrawCommands)) && (cmd != nullptr));
						}
						ImGui::TreePop();
					}
				}
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Main
{
	static HWND* windowHandle{};
	static int* chr_tex_num{};

	namespace FuncDefs
	{
		typedef LRESULT (*WINAPI WindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		typedef int (*GameInit_UDG)();
		typedef void (*FatalError)(const char* format, ...);

		typedef void (*GameInit)();

		typedef void (*UpdatePlayerInputs)(int unknown);
	}

	FuncDefs::WindowProc WindowProc{};

	FuncDefs::GameInit_UDG GameInit_UDG{};
	FuncDefs::FatalError FatalError{};

	FuncDefs::GameInit GameInit{};

	FuncDefs::UpdatePlayerInputs UpdatePlayerInputs{};

	namespace HookedFuncs
	{
		LRESULT WINAPI WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) { return TRUE; }
			Main::WindowProc(hwnd, msg, wParam, lParam);
		}

		void GameInit()
		{
			/*if (GameInit_UDG() >= 0 && *chr_tex_num > 4)
			{
				FatalError("chr_tex_num is greater than 4 (current value: %d)", *chr_tex_num);
				return;
			}*/

			GameInit_UDG();

			DebugTextLength = reinterpret_cast<int*>(baseAddr + 0x7ED534);
			DebugTextBuffer = reinterpret_cast<char*>(baseAddr + 0x7ED540);

			LogInfo("Gfx", "Max draw commands: %d", *Gfx::MaxDrawCommands);
			LogInfo("UDGHook", "Game initialized");

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			ImGui_ImplWin32_Init(*windowHandle);
			ImGui_ImplDX11_Init(*Gfx::d3dDevice, *Gfx::d3dDevContext);
			LogInfo("UDGHook", "ImGui initialized");
		}

		void UpdatePlayerInputs(int unknown)
		{
			ImGuiIO& io = ImGui::GetIO();

			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Insert, false)) { ImGuiActive = !ImGuiActive; }
			if (ImGuiActive && (io.WantCaptureKeyboard || io.WantCaptureMouse)) { return; }

			Main::UpdatePlayerInputs(unknown);
		}
	}
}

bool InitGfx()
{
	Gfx::FirstDrawCommand = reinterpret_cast<Gfx::DrawCommand_Base*>(baseAddr + 0x826490);
	Gfx::MaxDrawCommands = reinterpret_cast<i16*>(baseAddr + 0x802B72);

	Gfx::UnbindVertexBuffer = reinterpret_cast<Gfx::FuncDefs::UnbindVertexBuffer>(baseAddr + 0x129800);
	Gfx::ExecuteCommandBuffer = reinterpret_cast<Gfx::FuncDefs::ExecuteCommandBuffer>(baseAddr + 0x124AA0);
	Gfx::RenderFrame = reinterpret_cast<Gfx::FuncDefs::RenderFrame>(baseAddr + 0x121D10);

	Gfx::DrawCommandFuncs = reinterpret_cast<Gfx::FuncDefs::DrawCommandFunc*>(baseAddr + 0x30AED0);

	Gfx::d3dDevice = reinterpret_cast<ID3D11Device**>(baseAddr + 0x82C190);
	Gfx::d3dDevContext = reinterpret_cast<ID3D11DeviceContext**>(baseAddr + 0x82C198);

	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	//if (DetourAttach(&(PVOID&)Gfx::ExecuteCommandBuffer, Gfx::HookedFuncs::ExecuteCommandBuffer) != NO_ERROR) { return false; }
	if (DetourAttach(&(PVOID&)Gfx::RenderFrame, Gfx::HookedFuncs::RenderFrame) != NO_ERROR) { return false; }

	DetourTransactionCommit();
	return true;
}

bool DestroyGfx()
{
	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&)Gfx::RenderFrame, Gfx::HookedFuncs::RenderFrame) != NO_ERROR) { return false; }

	DetourTransactionCommit();
	return true;
}

bool InitMain()
{
	Main::windowHandle = reinterpret_cast<HWND*>(baseAddr + 0x8264C0);
	Main::chr_tex_num = reinterpret_cast<int*>(baseAddr + 0x802B24);

	Main::WindowProc = reinterpret_cast<Main::FuncDefs::WindowProc>(baseAddr + 0xF6320);

	Main::GameInit_UDG = reinterpret_cast<Main::FuncDefs::GameInit_UDG>(baseAddr + 0x85850);
	Main::FatalError = reinterpret_cast<Main::FuncDefs::FatalError>(baseAddr + 0x10F9C0);

	Main::GameInit = reinterpret_cast<Main::FuncDefs::GameInit>(baseAddr + 0x1052A0);

	Main::UpdatePlayerInputs = reinterpret_cast<Main::FuncDefs::UpdatePlayerInputs>(baseAddr + 0x105AD0);

	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourAttach(&(PVOID&)Main::GameInit, Main::HookedFuncs::GameInit) != NO_ERROR) { return false; }
	if (DetourAttach(&(PVOID&)Main::WindowProc, Main::HookedFuncs::WindowProc) != NO_ERROR) { return false; }
	if (DetourAttach(&(PVOID&)Main::UpdatePlayerInputs, Main::HookedFuncs::UpdatePlayerInputs) != NO_ERROR) { return false; }

	DetourTransactionCommit();
	return true;
}

bool DestroyMain()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&)Main::GameInit, Main::HookedFuncs::GameInit) != NO_ERROR) { return false; }
	if (DetourDetach(&(PVOID&)Main::WindowProc, Main::HookedFuncs::WindowProc) != NO_ERROR) { return false; }
	if (DetourDetach(&(PVOID&)Main::UpdatePlayerInputs, Main::HookedFuncs::UpdatePlayerInputs) != NO_ERROR) { return false; }

	DetourTransactionCommit();
	return true;
}

void FatalError(const char* message)
{
	MessageBoxA(NULL, "UDGHook - Error", message, MB_ICONERROR);
}

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
	baseAddr = (offset_t)(GetModuleHandle(NULL));

#ifdef _DEBUG
	if (InitConsoleOutput() == 0)
	{
		FatalError("Failed to allocate console");
		return false;
	}
#endif // _DEBUG

	LogInfo("UDGHook", "Console output initialized");

	InitGfx();
	LogInfo("UDGHook", "Hooked into GFX");

	InitMain();
	LogInfo("UDGHook", "Hooked main functionality");

	return true;
}

void DestroyHooks()
{
#ifdef _DEBUG
	CloseConsoleOutput();
#endif // _DEBUG

	DestroyMain();
	DestroyGfx();
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
