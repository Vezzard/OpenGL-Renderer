#include "pch.h"
#include "RenderCommand.h"

#if VULKAN
#include "Platform/Vulkan/VulkanRendererAPI.h"
#else
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#endif

namespace Engine {

#if VULKAN
	RendererAPI* RenderCommand::s_RendererAPI = new VulkanRendererAPI;
#else
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
#endif
	

}