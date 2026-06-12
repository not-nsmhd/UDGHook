#pragma once
#include "Common/Types.h"

namespace UDGHook::Main
{
	struct HookWindow : NonCopyable
	{
	public:
		HookWindow() = default;
		~HookWindow() = default;

	public:
		virtual void OnDisplay() = 0;
		virtual std::string_view GetMenuItemName() const = 0;

	public:
		bool IsVisible{ false };
	};
}
