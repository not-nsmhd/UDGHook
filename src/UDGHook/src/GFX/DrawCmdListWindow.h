#pragma once
#include "DrawCommands.h"
#include "Main/HookWindow.h"

namespace UDGHook::GFX
{
	class DrawCmdListWindow : public Main::HookWindow
	{
	public:
		DrawCmdListWindow();

	public:
		void OnDisplay() override;
		std::string_view GetMenuItemName() const override;

	private:
		DrawCommand* firstDrawCmd{};
		i16 maxDrawCmds{};
	};

	namespace Util
	{
		void ConstructDrawCommandInfoToDisplay(const DrawCommand* cmd);
	}
}
