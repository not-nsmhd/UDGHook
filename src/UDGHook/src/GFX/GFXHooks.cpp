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

	void HookedFuncs::RenderFrame()
	{
		ogFuncs.RenderFrame();

		/*static constexpr const char debugMessage[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
		strncpy_s(DebugTextBuffer, 2048, debugMessage, sizeof(debugMessage));
		*DebugTextLength = sizeof(debugMessage);*/

		/*ImGui_ImplWin32_NewFrame();
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
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/
	}
}
