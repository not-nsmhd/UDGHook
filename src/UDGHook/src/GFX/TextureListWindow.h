#pragma once
#include "Main/HookWindow.h"
#include "Texture.h"
#include <memory>

namespace UDGHook::GFX
{
	class TextureViewerWindow;

	class TextureListWindow : public Main::HookWindow
	{
	public:
		TextureListWindow();

	public:
		void OnDisplay() override;
		std::string_view GetMenuItemName() const override;

	private:
		const Texture* loadedTextures{};
		const bool_t* textureExistence{};
		char** textureNames{};

		std::unique_ptr<TextureViewerWindow> texViewer{};
	};

	class TextureViewerWindow : NonCopyable
	{
	public:
		TextureViewerWindow();

	public:
		void OnDisplay();
		void SetTextureToDisplay(const Texture* tex, const char* name, const bool_t* existence);

	public:
		bool IsVisible{};
		int TexPageIndex{ -1 };

	private:
		const Texture* texToDisplay{};
		const bool_t* texExistence{};
		char texName[MaxTextureNameLength]{};

		int texDisplaySize{ 128 };
		bool texBg{ false };
	};
}
