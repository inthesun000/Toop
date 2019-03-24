#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "BackendVulkan.h"

class HelloTriangleApplication
{
public:
	void Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();
	}

private:
	void InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "vulkan", nullptr, nullptr);
	}

	void InitVulkan()
	{
		//MUSTDO: Sun develop memory allocator
		vulkan = new BackendVulkan();

		VkApplicationInfo appInfo = { };
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Hexa";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		vulkan->CreateInstance(createInfo);
	}

	void MainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			// catch events
			glfwPollEvents();
		}
	}

	void CleanUp()
	{
		//MUSTDO: Sun -- develop memory allocator
		delete vulkan;

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
private:
	BackendVulkan * vulkan = nullptr;
	GLFWwindow * window = nullptr;

	const int WIDTH = 800;
	const int HEIGHT = 600;
};

int main()
{
	HelloTriangleApplication app;
	
	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}