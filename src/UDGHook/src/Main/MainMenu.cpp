#include "MainMenu.h"
#include <memory>

#include "ImGui/imgui.h"
#include "GFX/DrawCmdListWindow.h"
#include "GFX/TextureListWindow.h"

namespace UDGHook::Main
{
	std::unique_ptr<MainMenu> Instance{};

	MainMenu::MainMenu()
	{
		gfxWindows.emplace_back(new GFX::DrawCmdListWindow());
		gfxWindows.emplace_back(new GFX::TextureListWindow());
	}

	MainMenu::~MainMenu()
	{
		for (size_t i = 0; i < gfxWindows.size(); i++)
		{
			delete gfxWindows[i];
		}
		gfxWindows.clear();
	}

	void MainMenu::CreateInstance()
	{
		if (Instance == nullptr) { Instance = std::make_unique<MainMenu>(); }
	}

	void MainMenu::DestoryInstance()
	{
		if (Instance != nullptr) { Instance = nullptr; }
	}

	MainMenu* MainMenu::GetInstance()
	{
		return Instance.get();
	}

	void MainMenu::Toggle()
	{
		isActive = !isActive;
	}

	void MainMenu::OnRender()
	{
		if (!isActive) { return; }

		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Graphics"))
			{
				for (auto& win : gfxWindows)
				{
					if (ImGui::MenuItem(win->GetMenuItemName().data(), nullptr, nullptr)) { win->IsVisible = true; }
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Input"))
			{
				ImGui::MenuItem("Disable game input when the menu is visible", nullptr, &disableInputsWhenActive);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& win : gfxWindows)
		{
			if (win->IsVisible) { win->OnDisplay(); }
		}
	}

	bool MainMenu::ShouldInputsBeDisabled()
	{
		return disableInputsWhenActive;
	}
}
