#include "Logging.h"
#include <cstdio>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

namespace UDGHook
{
	static bool consoleExists = false;
	static bool consoleTested = false;

	bool ConsoleExists()
	{
#ifdef _WIN32
		if (consoleTested) { return consoleExists; }
		else { consoleExists = GetConsoleWindow() != NULL; return consoleExists; }
#endif // _WIN32
		return false;
	}

	void LogMessage(const char* text, ...)
	{
#ifdef _DEBUG
		va_list list;

		va_start(list, text);

		if (ConsoleExists())
		{
			vprintf(text, list);
			putchar('\n');
		}

		va_end(list);
#endif
	}

	void LogInfo(const char* component, const char* text, ...)
	{
#ifdef _DEBUG
		va_list list;

		va_start(list, text);

		if (ConsoleExists())
		{
			printf("[%s][INFO] ", component);
			vprintf(text, list);
			putchar('\n');
		}

		va_end(list);
#endif
	}

	void LogWarn(const char* component, const char* text, ...)
	{
#ifdef _DEBUG
		va_list list;

		va_start(list, text);

		if (ConsoleExists())
		{
			printf("[%s][WARN] ", component);
			vprintf(text, list);
			putchar('\n');
		}

		va_end(list);
#endif
	}

	void LogError(const char* component, const char* text, ...)
	{
#ifdef _DEBUG
		va_list list;

		va_start(list, text);

		if (ConsoleExists())
		{
			printf("[%s][ERROR] ", component);
			vprintf(text, list);
			putchar('\n');
		}

		va_end(list);
#endif
	}
}
