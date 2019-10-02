#include "pch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine {

	SPtr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(path);
		}

		CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Engine::SPtr<Engine::Texture2D> Texture2D::Create(unsigned char* data, int width, int height, int channels)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(data, width, height, channels);
		}

		CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

   Engine::SPtr<Engine::CubeMap> CubeMap::Create(const std::vector<std::string>& paths)
   {
      switch (Renderer::GetAPI())
      {
      case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
      case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLCubeMap>(paths);
      }

      CORE_ASSERT(false, "Unknown RendererAPI!");
      return nullptr;
   }

}