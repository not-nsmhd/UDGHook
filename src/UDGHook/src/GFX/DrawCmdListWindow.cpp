#include "DrawCmdListWindow.h"
#include "GFXHooks.h"
#include "ImGui/imgui.h"

namespace UDGHook::GFX
{
	DrawCmdListWindow::DrawCmdListWindow()
	{
		firstDrawCmd = GFX::GetOGData()->FirstDrawCommand;
		maxDrawCmds = *GFX::GetOGData()->MaxDrawCommands;
	}

	void DrawCmdListWindow::OnDisplay()
	{
		size_t maxDrawCmds_sizet = static_cast<size_t>(maxDrawCmds);
		DrawCommand* cmd = firstDrawCmd;

		int commandIndex = 0;
		ImGui::Begin("Draw Commands", &IsVisible);
		{
			do
			{
				if (cmd->Type != DrawCommandType::None)
				{
					ImGui::PushID(commandIndex);
					{
						const std::string_view& typeName = DrawCommandTypeNames[static_cast<size_t>(cmd->Type)];
						
						if (ImGui::TreeNode("", "Command %02d (%s)", commandIndex++, typeName.data()))
						{
							Util::ConstructDrawCommandInfoToDisplay(cmd);
							ImGui::TreePop();
						}
					}
					ImGui::PopID();
				}
				cmd = cmd->Next;
			} while (cmd != (firstDrawCmd + 16 * maxDrawCmds_sizet) && cmd != nullptr);
		}
		ImGui::End();
	}

	std::string_view DrawCmdListWindow::GetMenuItemName() const
	{
		return "(Debug) Draw Commands";
	}

	void Util::ConstructDrawCommandInfoToDisplay(const DrawCommand* cmd)
	{
		if (cmd == nullptr) { return; }
		switch (cmd->Type)
		{
		case DrawCommandType::SingleSprite:
		{
			const DrawCommand_SingleSpriteData* cmdData = reinterpret_cast<const DrawCommand_SingleSpriteData*>(&cmd->Data);

			ImGui::Text("Texture Page: %d", cmdData->TexPage);
			ImGui::Text("Flags: 0x%X", cmdData->Flags);
			ImGui::Text("Sprite Offset: %d", cmdData->SpriteOffset);

			break;
		}
		case DrawCommandType::MultipleSprites:
		{
			const DrawCommand_MultipleSpritesData* cmdData = reinterpret_cast<const DrawCommand_MultipleSpritesData*>(&cmd->Data);

			ImGui::Text("Texture Page: %d", cmdData->TexPage);
			ImGui::Text("Flags: 0x%X", cmdData->Flags);
			ImGui::Text("Sprite Offset: %d", cmdData->SpriteOffset);
			ImGui::Text("Sprite Count: %d", cmdData->SpriteCount);

			break;
		}
		}
	}
}
