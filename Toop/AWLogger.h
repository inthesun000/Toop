#pragma once

namespace Log
{
	enum LEVEL
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

	enum TASK
	{
		Application,
		EngineInitialization,
		GraphicAPI,
		WorldInfo,
		Drawing
	};

	struct Message
	{
		LEVEL level;
		TASK task;
		//MUSTDO: [Sun] You Support Unicode characters
		char* message = nullptr;
		Message(LEVEL _level, TASK _task,char* _message)
		{
			level = level;
			message = message;
			task = _task;
		}
	};

	enum COLOR
	{
		WHITE = 7,
		GRAY = 8,
		YELLOW = 14,
		RED = 12,
		MINT = 11,
		GRAYMINT = 3
	};


	class AWLogger
	{
	private:
		void* console;

	private:
		int Coloring(LEVEL level);
		int Coloring(COLOR color);
		void PrintConsole(const char* message, bool useEndline);
		const char* AttachWord(LEVEL level);
		const char* AttachWord(TASK task);
		const char* AttachWord(State state);

	public:
		AWLogger();
		~AWLogger();

		void Logging(const Message message);
		void Logging(LEVEL level, TASK task, State state, const char* message);

	};
}