
#include "AWInclude.h"
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

		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::Application, Log::State::Done, "...Created Window");
	}

	void InitVulkan()
	{
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::Application, Log::State::Initialization, "...Create Vulkan");

		//MUSTDO: [Sun] develop memory allocator
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
		vulkan->SetupDebugMessenger();

		vulkan->RememberWindowSize(WIDTH, HEIGHT);
		vulkan->CreateSurface(glfwGetWin32Window(window), GetModuleHandle(nullptr));

		vulkan->PickPhysicalDevice();
		vulkan->CreateLogicalDevice();

		vulkan->CreateSwapChain();
		vulkan->CreateImageViews();

		vulkan->CreateRenderPass();
		vulkan->CreateGraphicsPipeline();

		vulkan->CreateFramebuffers();
		vulkan->CreateCommandPool();
		vulkan->CreateVertexBuffer();
		vulkan->CreateCommandBuffers();

		vulkan->CreateSyncObjects();
	}

	void DrawFrame()
	{
		vulkan->Draw();
	}

	void MainLoop()
	{
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::Application, Log::State::Initialization, "...Start Main loop");
		while (!glfwWindowShouldClose(window))
		{
			// catch events
			glfwPollEvents();
			DrawFrame();
		}

		WaitDeviceIdle();
	}

	void WaitDeviceIdle()
	{
		vulkan->WaitDeviceIdle();
	}

	void CleanUp()
	{
		//MUSTDO: [Sun] develop memory allocator
		delete vulkan;

		glfwDestroyWindow(window);
		glfwTerminate();

		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::Application, Log::State::Done, "...CleanUp Application");
	}
public:
	const int WIDTH = 800;
	const int HEIGHT = 600;
	
private:
	BackendVulkan * vulkan = nullptr;
	GLFWwindow * window = nullptr;
	Log::AWLogger localLogger;

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