#pragma once

namespace UDGHook
{
	bool ConsoleExists();

	void LogMessage(const char* text, ...);
	void LogInfo(const char* component, const char* text, ...);
	void LogWarn(const char* component, const char* text, ...);
	void LogError(const char* component, const char* text, ...);
}
