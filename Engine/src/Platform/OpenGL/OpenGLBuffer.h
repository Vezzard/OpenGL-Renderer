#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		virtual uint GetSize() const override { return m_size; }
	private:
		uint m_RendererID;
		BufferLayout m_Layout;
		uint m_size;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint* indices, uint count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint GetCount() const { return m_Count; }
	private:
		uint m_RendererID;
		uint m_Count;
	};


	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer();
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddTexture(const SPtr<Texture>& texture) override;
		virtual void AddRenderBuffer(const SPtr<RenderBuffer>& rb) override;

		virtual bool Check() const override;

	private:
		uint m_RendererID;
		uint m_ColorTextureOffset = 0;

		std::vector<SPtr<RenderBuffer>> m_RenderBuffers;
	};


	class OpenGLRenderBuffer : public RenderBuffer
	{
	public:
		OpenGLRenderBuffer(uint windth, uint heigth);
		virtual ~OpenGLRenderBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		uint GetRendererID() const { return m_RendererID; }

	private:
		uint m_RendererID;
	};

}