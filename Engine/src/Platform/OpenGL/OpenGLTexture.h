#pragma once

#include "Engine/Renderer/Texture.h"

namespace Engine {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(unsigned char* data, int width, int height, int channels) { Init(data, width, height, channels); } 
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width;  }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		void Init(unsigned char* data, int width, int height, int channels);

		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};

}
