#include "pch.h"
#include "RendererAPI.h"

namespace Engine {
#if VULKAN
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
#else
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif

}