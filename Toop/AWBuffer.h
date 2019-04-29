#pragma once
#include "AWCore.h"

class AWBuffer
{
public:
	AWBuffer();
	virtual ~AWBuffer();

private:
	VkBuffer buffer;
	VkDeviceMemory memory;
};

