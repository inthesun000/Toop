//#include <vulkan/vulkan.h>

#include <iostream>
#include "VkFoundation.h"


class DrawTriangleApp
{
private:
	VkFoundation * foundation=nullptr;

private:
	void InitVulkan()
	{
		foundation->CreateInstance();
		foundation->SetupDebugCallback();
	}

	void Loop()
	{
		foundation->EventLoop();
	}

	void CleanUp()
	{
		delete foundation;
	}

	void InitWindow()
	{	
		foundation = new VkFoundation;
	}

public:
	void Run()
	{
		InitWindow();
		InitVulkan();
		Loop();
		CleanUp();
	}
};

int main()
{
	DrawTriangleApp app;

	try {
		app.Run();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}