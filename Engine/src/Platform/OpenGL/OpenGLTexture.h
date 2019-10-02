#pragma once

#include "Engine/Renderer/Texture.h"

namespace Engine {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(unsigned char* data, int width, int height, int channels) { Init(data, width, height, channels); } 
		virtual ~OpenGLTexture2D();

		virtual uint GetWidth	   (void) const override   { return m_Width;  }
		virtual uint GetHeight     (void) const override   { return m_Height; }
                   
		virtual void Bind		      (uint32_t slot = 0) const override;
		virtual uint GetRenderId	(void) const override   { return m_RendererID; };
	private:
		void Init(unsigned char* data, int width, int height, int channels);

		std::string m_Path;
		uint m_Width, m_Height;
		uint m_RendererID;
	};


   class OpenGLCubeMap : public CubeMap
   {
   public:
      OpenGLCubeMap(const std::vector<std::string>& paths);
      virtual ~OpenGLCubeMap();
      
      virtual void Bind          (uint32_t slot = 0) const override;
      virtual uint GetRenderId   (void) const override   { return m_RendererID; };

		virtual uint GetWidth	   (void) const override   { return m_Width;  }
		virtual uint GetHeight     (void) const override   { return m_Height; }

   private:
      uint m_Width, m_Height;
      uint m_RendererID;
   };

}
