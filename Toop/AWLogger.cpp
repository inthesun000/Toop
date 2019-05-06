#include "AWInclude.h"
#include "AWLogger.h"

using namespace Log;

AWLogger::AWLogger() 
{
	console = GetStdHandle(STD_OUTPUT_HANDLE);
}


AWLogger::~AWLogger()
{
}

void AWLogger::Logging(const Log::Message message)
{
	Coloring(message.level);
	PrintConsole(AttachWord(message.task), false);
	PrintConsole(message.message, true);
}

void AWLogger::Logging(Level level, Task task, State state, const char* message)
{
	PrintConsole(AttachWord(task), false);

	Coloring(Color::Mint);
	PrintConsole(AttachWord(state), false);

	Coloring(level);
	PrintConsole(message, true);
}

int AWLogger::Coloring(Log::Level level)
{
	Color color = Color::White;
	switch (level)
	{
	case Level::Normal:
		color = Color::White;
		break;
	case Level::Warring:
		color = Color::Yellow;
		break;
	case Level::Error:
		color = Color::Red;
		break;
	default:
		color = Color::Gray;
		break;
	}

	FlushConsoleInputBuffer(console);
	SetConsoleTextAttribute(console, color);

	return color;
}


int AWLogger::Coloring(Log::Color color)
{
	FlushConsoleInputBuffer(console);
	SetConsoleTextAttribute(console, static_cast<int>(color));

	return static_cast<int>(color);
}


void Log::AWLogger::PrintConsole(const char * message, bool useEndline)
{
	if (useEndline)
		std::cout << message << std::endl;
	else
		std::cout << message;
}

const char * Log::AWLogger::AttachWord(Level level)
{
	switch (level)
	{
	case Log::Error:
		return "[Error]";

	case Log::Warring:
		return "[Warring]";

	case Log::Normal:
		return "[Normal]";

	default:
		break;
	}
}

const char* Log::AWLogger::AttachWord(Task task)
{
	switch (task)
	{
	case Log::EngineInitialization:
		return "[EngineInitialization]";
	case Log::GraphicAPI:
		return "[GraphicAPI]";
	case Log::WorldInfo:
		return "[WorldInfo]";
	case Log::Drawing:
		return "[Drawing]";
	case Log::Application:
		return "[Application]";
	default:
		return nullptr;
	}

	return nullptr;
}

const char * Log::AWLogger::AttachWord(State state)
{
	switch (state)
	{
	case Log::State::Initialization:
		return "---Initialization---";

	case Log::State::Progress:
		return "---Progress---";

	case Log::State::Done:
		return "---Done---";
	default:
		return nullptr;
	}
	return nullptr;
}
