#include "BackendVulkan.h"

#include <stdexcept>
#include <vector>
#include <iostream>

BackendVulkan::BackendVulkan()
{
}


BackendVulkan::~BackendVulkan()
{
	DestroyVulkan();
}

void BackendVulkan::DestroyVulkan()
{
	vkDestroyInstance(instance, nullptr);
}

void BackendVulkan::CreateInstance(const VkInstanceCreateInfo& createInfo)
{
	result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		//@TODO: error check --> validation layers
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
}