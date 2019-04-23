#include "BackendVulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <limits>
#include <set>
#include <algorithm>

VkVertexInputBindingDescription VertexData::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexData);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> VertexData::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(VertexData, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(VertexData, color);

	return attributeDescriptions;
}


BackendVulkan::BackendVulkan()
{
}


BackendVulkan::~BackendVulkan()
{
	DestroyVulkan();
}

void BackendVulkan::DestroyVulkan()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, renderFinishSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}
	vkDestroyCommandPool(device, commandPool, nullptr);

	for (auto framebuffer : swapChainFramebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	vkDestroyPipeline(device, graphicPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);

}

VkResult BackendVulkan::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void BackendVulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "VkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

void BackendVulkan::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType =	VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity=	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT	|
								VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType=	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT	|
							VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT	|
							VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT	;

	createInfo.pfnUserCallback = DebugCallBack;
	createInfo.pUserData = nullptr;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)
		!= VK_SUCCESS)
		throw std::runtime_error("failed to set up debug messenger!");
	else
		//TODO: [Sun] Trace function progress
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...CreateDebugUtilsMessengerEXT");
}

const bool BackendVulkan::IsDeviceSuitable(VkPhysicalDevice device)
{	
	if (device == VK_NULL_HANDLE)
		return false;

	// for example, Vulkan only support on supported Geometry shader;
	/*
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;*/

	QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionSupported)
	{
		SwapChainSupportDetails swapChaindSupportDetail = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChaindSupportDetail.formats.empty() && !swapChaindSupportDetail.presentModes.empty();
	}

	return indices.IsComplete() && extensionSupported && swapChainAdequate;
}

VkSurfaceFormatKHR BackendVulkan::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 &&
		availableFormats[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR BackendVulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentMode;
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D BackendVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width = std::clamp(actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);

		actualExtent.height = std::clamp(actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);
	}
	return capabilities.currentExtent;
}

const bool BackendVulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtension(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtension.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtension)	
		requiredExtensions.erase(extension.extensionName);
	
	return requiredExtensions.empty();
}

void BackendVulkan::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	/*if(physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	*/
	//TODO: [Sun] Trace function progress
	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...PickVulkanPhysicalDevice");
}

void BackendVulkan::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);


	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	float queuePriority = 1.0f;
	for (auto queueFamliy : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo	createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");
	else
		//TODO: [Sun] Trace function progress
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...CreateVulkanLogicalDevice");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	
}

void BackendVulkan::CreateSurface(const HWND windowHandle, const HINSTANCE hInstance)
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = windowHandle;
	createInfo.hinstance = hInstance;

	if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
	else
		//TODO: [Sun] Trace function progress
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...CreateSurface");
}

void BackendVulkan::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;

	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
						// 명백한 소유권 이전없이 여러 큐패밀리에서 이미지 사용가능
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{	/* 이미지는 한번에 하나의 대기열 패밀리가 소유, 
		다른 대기열 패밀리에서 사용하기 전에 소유권을 명시적으로 전송해야 함.
		*/
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //optional
		createInfo.pQueueFamilyIndices = nullptr; //optional
	}
	
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}
	
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	//TODO: [Sun] Trace function progress
	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...CreateVulkanSwapchain");
}

void BackendVulkan::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) 
			!= VK_SUCCESS)
			throw std::runtime_error("failed to create image views!");
		
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Image view");
	}
}

void BackendVulkan::CreateGraphicsPipeline()
{
	/* Step 0 : Create Shader Module */
	auto vertShaderCode = FileManager::ReadFile("vert.spv");
	auto fragShaderCode = FileManager::ReadFile("frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	/* Step 1 : Create Shader stage */
	VkPipelineShaderStageCreateInfo vertShaderStageInfo =
		CreateShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo =
		CreateShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule);

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	/* Step 2 : Input vertex */

	auto bindingDescriptions = VertexData::GetBindingDescription();
	auto attributeDescriptions = VertexData::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions; // optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // optional

	/* Step 3: Input Assembly */
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	/* Step 4 : Viewport and Scissors */
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;

	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	/* Step 5: Rasterizer */
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;

	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

	rasterizer.lineWidth = 1.0f;

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	/* Step 6: MultiSampling */
	VkPipelineMultisampleStateCreateInfo multiSampling = {};
	multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampling.sampleShadingEnable = VK_FALSE;
	multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multiSampling.minSampleShading = 1.0f;
	multiSampling.pSampleMask = nullptr;
	multiSampling.alphaToCoverageEnable = VK_FALSE;
	multiSampling.alphaToOneEnable = VK_FALSE;

	/* Step 7 : Depth and Stencil */

	/* Step 8 : Color blending */
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	
	for (auto& blendContant : colorBlending.blendConstants)
		blendContant = 0.0f;
	
	/* Step 9 : Dynamic state */
	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	/* Step 10 : Pipeline layout */
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	//pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	//pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout)
		!= VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout! ");

	/* Step 11 : Create Pipeline */
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multiSampling;
	pipelineInfo.pColorBlendState = &colorBlending;

	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicPipeline)
		!= VK_SUCCESS)
		throw std::runtime_error("failed to create graphic pipeline!");

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Graphics Pipeline");
}

void BackendVulkan::CreateFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i])
			!= VK_SUCCESS)
			throw std::runtime_error("failed to create frame buffer!");

		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Created frame buffer");
	}
}

void BackendVulkan::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	createInfo.flags = 0;

	if (vkCreateCommandPool(device, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Command pool");
}

void BackendVulkan::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());
	VkCommandBufferAllocateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	createInfo.commandPool = commandPool;
	createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	createInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &createInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to create command buffers!");

	//TODO: [Sun] Logging Progress
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline);
		
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(verties.size()), 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create command buffers");
}

void BackendVulkan::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if ((vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i])
			!= VK_SUCCESS) 
			||(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishSemaphores[i])
				!= VK_SUCCESS)
			||(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i])
				!= VK_SUCCESS))
			throw std::runtime_error("failed to create semaphore!");
	}
	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Semaphores");
}

void BackendVulkan::WaitDeviceIdle()
{
	vkDeviceWaitIdle(device);
}

void BackendVulkan::Draw()
{
	vkWaitForFences(device, 1, &inFlightFences[CurrentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
	uint32_t imageIndex;
	vkResetFences(device, 1, &inFlightFences[CurrentFrame]);

	vkAcquireNextImageKHR(device, swapChain, (std::numeric_limits<uint64_t>::max)(),
		imageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	/* Submit Command buffer */
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	
	VkSemaphore signalSemaphores[] = { renderFinishSemaphores[CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to Submit draw command buffer!");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(presentQueue, &presentInfo);
	vkQueueWaitIdle(presentQueue);

	CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void BackendVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};

	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate buffer memory!");

	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create buffer");
}

uint32_t BackendVulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) 
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void BackendVulkan::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(verties[0]) * verties.size();

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertexBuffer, vertexBufferMemory);

	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verties.data(), (size_t)bufferSize);
	vkUnmapMemory(device, vertexBufferMemory);

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Vertex buffer");
}


void BackendVulkan::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	/*
	colorAttachment.stencilLoadOp;
	colorAttachment.stencilStoreOp;
	...
	*/
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subPass = {};
	subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subPass.colorAttachmentCount = 1;
	subPass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subPass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Render pass");
}

VkShaderModule BackendVulkan::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...Create Shader Module");

	return shaderModule;
}

VkPipelineShaderStageCreateInfo BackendVulkan::CreateShaderStageInfo(VkShaderStageFlagBits type, VkShaderModule & module)
{
	VkPipelineShaderStageCreateInfo newCreateInfo = {};

	newCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	newCreateInfo.stage = type;
	newCreateInfo.module = module;
	newCreateInfo.pName = "main";

	return newCreateInfo;
}

const bool BackendVulkan::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}
	return true;
}

std::vector<const char*> BackendVulkan::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}


void BackendVulkan::CreateInstance(const VkInstanceCreateInfo& _createInfo)
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
		throw std::runtime_error("validation layers requested, but not available!");

	VkInstanceCreateInfo createInfo = _createInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	

	result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		//TODO: error check --> validation layers
		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			std::cout << "available extensions: " << std::endl;

			for (const auto& extension : extensions)
			{
				std::cout << "\t" << extension.extensionName << std::endl;
			}
		}

		throw std::runtime_error("failed to create instance!");
	}
	else
		//TODO: [Sun] Trace function progress
		localLogger.Logging(Log::LEVEL::Normal, Log::TASK::GraphicAPI, Log::State::Done, "...CreateVulkanInstance");
}

VKAPI_ATTR VkBool32 VKAPI_CALL BackendVulkan::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{ 
		std::cerr << "MessageSeverity : ERROR	";
	}
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		std::cerr << "MessageSeverity : WARRING	";
	}

	std::cerr << "Validation layer :" << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

BackendVulkan::QueueFamilyIndices BackendVulkan::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
			indices.presentFamily = i;

		if(indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

BackendVulkan::SwapChainSupportDetails BackendVulkan::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails detail;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &detail.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount > 0)
	{
		detail.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, detail.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount > 0)
	{
		detail.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, detail.presentModes.data());
	}


	return detail;
}

