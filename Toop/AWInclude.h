#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <optional>
#include <set>
#include <functional>
#include <cstdlib>
#include <array>
#include <limits>
#include <algorithm>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#if _WIN32
#include "Windows.h"
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define  GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


