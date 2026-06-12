#include "TextureListWindow.h"
#include "GFXHooks.h"
#include "Main/MainHooks.h"
#include "ImGui/imgui.h"

namespace UDGHook::GFX
{
	TextureListWindow::TextureListWindow() : texViewer{ std::make_unique<TextureViewerWindow>() }
	{
		auto ogData = GFX::GetOGData();
		loadedTextures = ogData->LoadedTextures;
		textureExistence = ogData->TextureExistence;
		textureNames = ogData->TextureNames;
	}

	void TextureListWindow::OnDisplay()
	{
		char fallbackTexNameBuffer[128]{};
		static constexpr float minListboxWidth{ 256 };

		ImGui::Begin("Texture Pages", &IsVisible);
		{
			const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
			const float listboxWidth = max(contentRegion.x, minListboxWidth);

			if (ImGui::BeginListBox("##texlistwindow-list", ImVec2(listboxWidth, contentRegion.y)))
			{
				for (size_t i = 0; i < MaxTexturePageIndex; i++)
				{
					if (!textureExistence[i]) { continue; }

					const Texture* tex = &loadedTextures[i];
					const char* texName = GetTextureNamePointer(textureNames, i);

					if (tex == nullptr) { continue; }

					if (texName[0] == 0)
					{
						// Texture name is not set, so let's construct our own!
						sprintf_s(fallbackTexNameBuffer, "Texture %llu (unnamed)", i);
						texName = fallbackTexNameBuffer;
					}

					ImGui::PushID(i);
					if (ImGui::Selectable(texName))
					{
						texViewer->SetTextureToDisplay(tex, texName, &textureExistence[i]);
						texViewer->TexPageIndex = i;
						texViewer->IsVisible = true;
					}
					ImGui::PopID();
				}
				ImGui::EndListBox();
			}
		}
		ImGui::End();

		texViewer->OnDisplay();
	}

	std::string_view TextureListWindow::GetMenuItemName() const
	{
		return "(Debug) Texture Pages";
	}

	TextureViewerWindow::TextureViewerWindow()
	{
	}

	void TextureViewerWindow::OnDisplay()
	{
		if (IsVisible)
		{
			ImGui::Begin("Texture Viewer", &IsVisible);
			{
				if (TexPageIndex == -1 || texToDisplay == nullptr || *texExistence == 0) 
				{
					ImGui::Text("Texture to display has not been set.\nChoose a texture to display in the texture list window.");
					ImGui::End();
					return;
				}
				
				ImGui::Text("Name: %s", texName);

				const int largestDimension = max(texToDisplay->Width, texToDisplay->Height);
				const int minDisplaySize = min(largestDimension, 128);
				const int maxDisplaySize = max(128, largestDimension);
				ImGui::SliderInt("Display Size", &texDisplaySize, minDisplaySize, maxDisplaySize);
				ImGui::Checkbox("Display Background", &texBg);

				ImTextureID texPtr = reinterpret_cast<ImTextureID>(texToDisplay->D3DShaderResourceView);

				const float dispScale = static_cast<float>(texDisplaySize) / static_cast<float>(largestDimension);

				const ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
				const ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right
				const ImVec4 bgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

				ImGui::ImageWithBg(texPtr, ImVec2(texToDisplay->Width * dispScale, texToDisplay->Height * dispScale),
					uv_min, uv_max, texBg ? bgColor : ImVec4());

				ImGui::Text("Dimensions: %dx%d", texToDisplay->Width, texToDisplay->Height);
				ImGui::Text("Bits per pixel: %d", texToDisplay->BitsPerPixel);
				ImGui::Text("Mipmaps: %d", texToDisplay->MipCount);

				const std::string_view& dxgiFormatName = DXGIFormatNames[static_cast<size_t>(texToDisplay->DXGIFormat)];
				ImGui::Text("DXGI Format: %s", dxgiFormatName.data());

				u32 texSize = (texToDisplay->TotalSize > 0) ? texToDisplay->TotalSize : (texToDisplay->Width * texToDisplay->Height * 4);
				ImGui::Text("Size: %u KiB", texSize / 1024);
			}
			ImGui::End();
		}
	}

	void TextureViewerWindow::SetTextureToDisplay(const Texture* tex, const char* name, const bool_t* existence)
	{
		texToDisplay = tex;
		texExistence = existence;
		strncpy_s(texName, name, sizeof(texName) - 1);
	}
}
