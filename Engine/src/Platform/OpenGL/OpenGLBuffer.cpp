#include "pch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint size)
		: m_size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint* indices, uint count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer()
	{
		glGenFramebuffers(1, &m_RendererID);
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::AddTexture(const SPtr<Texture>& texture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_ColorTextureOffset++, GL_TEXTURE_2D, texture->GetRenderId(), 0);
	}

	void OpenGLFrameBuffer::AddRenderBuffer(const SPtr<RenderBuffer>& rb)
	{
		OpenGLRenderBuffer* oglRB = static_cast<OpenGLRenderBuffer*>(rb.get());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, oglRB->GetRendererID());

		m_RenderBuffers.emplace_back(rb);
	}

	bool OpenGLFrameBuffer::Check() const
	{
		return (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);
	}

	OpenGLRenderBuffer::OpenGLRenderBuffer(uint windth, uint heigth)
	{

		glGenRenderbuffers(1, &m_RendererID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windth, heigth);
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		glDeleteRenderbuffers(1, &m_RendererID);
	}

	void OpenGLRenderBuffer::Bind() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	}

	void OpenGLRenderBuffer::Unbind() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

}