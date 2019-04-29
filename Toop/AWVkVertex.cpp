#include "AWVkVertex.h"


AWVkVertex::AWVkVertex()
{
}

AWVkVertex::~AWVkVertex()
{
}

VkVertexInputBindingDescription AWVkVertex::GetVertexBindingDescription()
{
	VkVertexInputBindingDescription desc;

	desc.binding = 0;
	desc.stride = sizeof(AWVertex);
	desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return desc;
}


std::array<VkVertexInputAttributeDescription, 2> AWVkVertex::GetVertexAttributeDescription()
{
	//TODO: How to use general perpose?
	std::array<VkVertexInputAttributeDescription, 2> attributeDesc = {};

	attributeDesc[0].binding = 0;
	attributeDesc[0].location = 0;
	attributeDesc[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDesc[0].offset = offsetof(AWVertex, AWVertex::pos);

	attributeDesc[1].binding = 0;
	attributeDesc[1].location = 0;
	attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDesc[1].offset = offsetof(AWVertex, AWVertex::color);

	return attributeDesc;
}

