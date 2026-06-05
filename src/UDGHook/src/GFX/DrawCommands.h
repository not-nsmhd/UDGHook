#pragma once
#include "Common/Types.h"

namespace UDGHook::GFX
{
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

	struct DrawCommand
	{
		DrawCommand* Next{};
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
}
