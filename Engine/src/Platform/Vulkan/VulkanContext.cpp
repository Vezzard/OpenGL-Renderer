#include "pch.h"
#include "VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Engine {

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void VulkanContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		
		INFO_CORE("Vulkan Info:");
		INFO_CORE("  extensionCount: {0}", extensionCount);
	}

	void VulkanContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}