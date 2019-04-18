﻿#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>

#include "Logger.h"
#include "FileManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

class BackendVulkan
{
public:
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		/* presentation은 Queue의 특정 기능이기 떄문에 
		실제로 우리가 만든 표면에 presentation을
		지원하는 큐 패밀리를 찾아야 함. 
		draw command를 지원하는 queue family와 presentation을 지원하는
		queue family는 overlap이 안될 수 있음.
		*/
		std::optional<uint32_t> presentFamily;

		const bool IsComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	/* 
	*	VkSurfaceCapabilitiesKHR
	*	VkSurfaceFormatKHR
	*	VkPresentModeKHR
	*/
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	enum ShaderType
	{
		Vertex,
		Fragment,
		Geometry,
		Compute
	};

public:
	BackendVulkan();
	~BackendVulkan();

	void CreateInstance(const VkInstanceCreateInfo& createInfo);
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	/*	TODO: VkWin32Surface로 표면 만들때 전처리기 말고, 윈도우용은 따로 해야하나? */
	void CreateSurface(const HWND windowHandle, const HINSTANCE hInstance);
	void CreateSwapChain();

	void CreateImageViews();
	void CreateRenderPass();

	void CreateGraphicsPipeline();
	void CreateFramebuffers();

	void CreateCommandPool();
	void CreateCommandBuffers();

	void CreateSyncObjects();
	void WaitDeviceIdle();

	void Draw();

	void RememberWindowSize(const uint32_t WIDTH, const uint32_t HEIGHT)
	{
		if (WIDTH > 0 && HEIGHT > 0)
		{
			this->WIDTH = WIDTH;
			this->HEIGHT = HEIGHT;
		}
	}

	/*
	@param VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : 진단 메시지
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT	: 자원 생성과 같은 정보 메시지
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARRING_BIT_EXT	: 반드시 오류는 아니지만 응용프로그램의 버그일 가능성이 있는 동작에 대한 메시지
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT	: 유효하지 않아 충돌을 일으킬 수 있는 동작에 대한 메시지
	
	@param VkDebugUtilsMessageTypeFlagsEXT messageType
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT		: 사양이나 성능과 관련이 없는 이벤트가 발생했습니다.
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT	: 사양을 위반하거나 실수를 나타낼 수 있는 문제가 발생했습니다.
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT	: Vulkan의 잠재적인 비최적 사용
	
	@param VkDebugUtilsMessengerCallbackDataEXT	pCallbackData
		pMessage	: null로 끝나는 문자열로서의 디버그 메시지
		pObjects	: 메시지와 관련된 Vulkan 개체 핸들의 배열
		objectCount	: 배열에 있는 객체의 수
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
		VkDebugUtilsMessageSeverityFlagBitsEXT	messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT			messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

protected:
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

private:
	void DestroyVulkan();

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT*	pCreateInfo,
		const VkAllocationCallbacks*	pAllocator,
		VkDebugUtilsMessengerEXT*		pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator
	);

	const bool	CheckValidationLayerSupport();
	const bool	CheckDeviceExtensionSupport(VkPhysicalDevice device);

	std::vector<const char*> GetRequiredExtensions();

	const bool IsDeviceSuitable(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits type, VkShaderModule& module);

private:
	VkInstance instance;
	VkResult result;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice	device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	std::vector<VkImage> swapChainImages;

	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishSemaphores;
	std::vector<VkFence> inFlightFences;

	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t WIDTH = 1;
	uint32_t HEIGHT = 1;

	size_t CurrentFrame = 0;

	//logger!!
	Log::Logger localLogger;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};
