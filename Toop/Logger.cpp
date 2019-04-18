#include "Logger.h"

#if _WIN32
#include "Windows.h"
#endif

using namespace Log;

Logger::Logger() 
{
	console = GetStdHandle(STD_OUTPUT_HANDLE);
}


Logger::~Logger()
{
}

void Logger::Logging(const Log::Message message)
{
	Coloring(message.level);
	PrintConsole(AttachWord(message.task), false);
	PrintConsole(message.message, true);
}

void Logger::Logging(LEVEL level, TASK task, State state, const char* message)
{
	PrintConsole(AttachWord(task), false);

	Coloring(COLOR::MINT);
	PrintConsole(AttachWord(state), false);

	Coloring(level);
	PrintConsole(message, true);
}

int Logger::Coloring(Log::LEVEL level)
{
	COLOR color = COLOR::WHITE;
	switch (level)
	{
	case LEVEL::Normal:
		color = COLOR::WHITE;
		break;
	case LEVEL::Warring:
		color = COLOR::YELLOW;
		break;
	case LEVEL::Error:
		color = COLOR::RED;
		break;
	default:
		color = COLOR::GRAY;
		break;
	}

	FlushConsoleInputBuffer(console);
	SetConsoleTextAttribute(console, color);

	return color;
}


int Logger::Coloring(Log::COLOR Color)
{
	FlushConsoleInputBuffer(console);
	SetConsoleTextAttribute(console, static_cast<int>(Color));

	return static_cast<int>(Color);
}


void Log::Logger::PrintConsole(const char * message, bool useEndline)
{
	if (useEndline)
		std::cout << message << std::endl;
	else
		std::cout << message;
}

const char * Log::Logger::AttachWord(LEVEL level)
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

const char* Log::Logger::AttachWord(TASK task)
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

const char * Log::Logger::AttachWord(State state)
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
