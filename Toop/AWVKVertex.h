#pragma once
#include "AWFoundation.h"
#include "AWInclude.h"

//template <typename Vertex>
struct AWVertex
{
	AWVec2<float> pos;
	AWVec3<float> color;

	AWVertex(AWVec2<float>& _pos, AWVec3<float>& _color)
	{
		pos = _pos;
		color = _color;
	}

	AWVertex(AWVec2<float>&& _pos, AWVec3<float>&& _color)
	{
		pos = _pos;
		color = _color;
	}
};


class AWVkVertex
{
private:
	AWVertex vertexData;
public:
	AWVkVertex() = delete;
	AWVkVertex(AWVertex& vertex) : vertexData(vertex) {};
	AWVkVertex(AWVertex&& vertex) : vertexData(vertex) {};
	~AWVkVertex();

	VkVertexInputBindingDescription GetVertexBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> GetVertexAttributeDescription();
};


