#pragma once
#include <vulkan/vulkan.h>

class BackendVulkan
{
public:
	BackendVulkan();
	~BackendVulkan();

	void CreateInstance(const VkInstanceCreateInfo& createInfo);

private:
	void DestroyVulkan();

private:
	VkInstance instance;
	VkResult result;

};

