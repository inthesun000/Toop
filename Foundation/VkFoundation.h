#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>


class VkFoundation
{
private:
	VkInstance instance;
	GLFWwindow * window;

	const int Width = 1200;
	const int Height = 800;

	const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

	VkDebugReportCallbackEXT callback;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

private:
	void InitWindow();
	void CleanUp();

public:
	VkFoundation() : window(nullptr) { InitWindow(); };
	~VkFoundation();

	void CreateInstance();
	void EventLoop();

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	void SetupDebugCallback();
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);
};

