#include "AWInclude.h"
#include "AWVkVertex.h"


VkVertexInputBindingDescription AWVertex::GetVertexBindingDescription()
{
	VkVertexInputBindingDescription desc;

	desc.binding = 0;
	desc.stride = sizeof(AWVertex);
	desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return desc;
}


std::array<VkVertexInputAttributeDescription, 3> AWVertex::GetVertexAttributeDescription()
{
	//TODO: How to use general perpose?
	std::array<VkVertexInputAttributeDescription, 3> attributeDesc = {};

	attributeDesc[0].binding = 0;
	attributeDesc[0].location = 0;
	attributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDesc[0].offset = offsetof(AWVertex,pos);

	attributeDesc[1].binding = 0;
	attributeDesc[1].location = 1;
	attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDesc[1].offset = offsetof(AWVertex,color);

	attributeDesc[2].binding = 0;
	attributeDesc[2].location = 2;
	attributeDesc[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDesc[2].offset = offsetof(AWVertex, texCoord);

	return attributeDesc;
}

