#pragma once
#include "AWFoundation.h"
#include "AWInclude.h"

//template <typename Vertex>
class AWVkVertex
{
private:
	AWVertex vertexData;

public:
	AWVkVertex();
	~AWVkVertex();

	VkVertexInputBindingDescription GetVertexBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> GetVertexAttributeDescription();
};


