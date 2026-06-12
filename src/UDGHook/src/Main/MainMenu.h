#pragma once
#include "Common/Types.h"
#include "HookWindow.h"
#include <vector>

namespace UDGHook::Main
{
	class MainMenu : NonCopyable
	{
	public:
		MainMenu();
		~MainMenu();

	public:
		static void CreateInstance();
		static void DestoryInstance();

		static MainMenu* GetInstance();

	public:
		void Toggle();
		void OnRender();

		bool ShouldInputsBeDisabled();

	private:
		bool isActive{};
		bool disableInputsWhenActive{ true };

		std::vector<HookWindow*> gfxWindows;
	};
}
