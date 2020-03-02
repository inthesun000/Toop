#pragma once
#include "AWFoundation.h"

//template <typename Vertex>
struct AWVertex
{
	AWVec3<float> pos;
	AWVec3<float> color;
	AWVec2<float> texCoord;

	AWVertex(AWVec3<float>& _pos, AWVec3<float>& _color, AWVec2<float>& _texCoord)
	{
		pos = _pos;
		color = _color;
		texCoord = _texCoord;
	}

	AWVertex(AWVec3<float>&& _pos, AWVec3<float>&& _color, AWVec2<float>&& _texCoord)
	{
		pos = _pos;
		color = _color;
		texCoord = _texCoord;
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

	static VkVertexInputBindingDescription GetVertexBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> GetVertexAttributeDescription();
};


