#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine {

VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLVertexBuffer(vertices, size);
	}

	CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLIndexBuffer(indices, size);
	}

	CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Engine::FrameBuffer* FrameBuffer::Create()
{
   switch (Renderer::GetAPI())
   {
   case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
   case RendererAPI::API::OpenGL:  return new OpenGLFrameBuffer();
   }

   CORE_ASSERT(false, "Unknown RendererAPI!");
   return nullptr;
}

Engine::RenderBuffer* RenderBuffer::Create(uint windth, uint heigth)
{
   switch (Renderer::GetAPI())
   {
   case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
   case RendererAPI::API::OpenGL:  return new OpenGLRenderBuffer(windth, heigth);
   }

   CORE_ASSERT(false, "Unknown RendererAPI!");
   return nullptr;
}

}