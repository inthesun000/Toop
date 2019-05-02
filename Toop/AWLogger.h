#pragma once

namespace Log
{
	enum Level
	{
		Error=0,
		Warring,
		Normal
	};

	enum State
	{
		Initialization,
		Progress,
		Done,		
	};

	enum Task
	{
		Application,
		EngineInitialization,
		GraphicAPI,
		WorldInfo,
		Drawing
	};

	struct Message
	{
		Level level;
		Task task;
		//MUSTDO: [Sun] You Support Unicode characters
		char* message = nullptr;
		Message(Level _level, Task _task,char* _message)
		{
			level = level;
			message = message;
			task = _task;
		}
	};

	enum Color
	{
		White = 7,
		Gray = 8,
		Yellow = 14,
		Red = 12,
		Mint = 11,
		GrayMint = 3
	};


	class AWLogger
	{
	private:
		void* console;

	private:
		int Coloring(Level level);
		int Coloring(Color color);
		void PrintConsole(const char* message, bool useEndLine);
		const char* AttachWord(Level level);
		const char* AttachWord(Task task);
		const char* AttachWord(State state);

	public:
		AWLogger();
		~AWLogger();

		void Logging(const Message message);
		void Logging(Level level, Task task, State state, const char* message);

	};
}