#pragma once

#include <string>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

namespace vkb {

	const std::string to_string(VkResult result);

}


namespace Cubed {

	ImGui_ImplVulkan_InitInfo* GetVulkanInfo();

}

/// @brief Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			throw std::runtime_error("Detected Vulkan error: " + vkb::to_string(err)); \
		}                                                                              \
	} while (0)
