#include "pch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>
#include "assimp/texture.h"

namespace Engine {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) {
			data = stbi_load("../assets/textures/black.png", &width, &height, &channels, 0);
		}
		CORE_ASSERT(data, "Failed to load image!");
		Init(data, width, height, channels);
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Init(unsigned char* data, int width, int height, int channels)
	{
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		switch (channels) {
		case 4: {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		} break;
		case 3: {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		} break;
		case 1: {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		} break;
		default:
			CORE_ASSERT(false, "Format not supported!");
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, GL_MAX_TEXTURE_MAX_ANISOTROPY);
	}

	void OpenGLTexture2D::Bind(uint slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>& paths)
	{
		ASSERT(paths.size() == 6);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		stbi_set_flip_vertically_on_load(0);
		int width, height, channels;
		for (uint i = 0; i < paths.size(); i++)
		{
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
			}
			else
			{
				ERROR_CORE("Cubemap texture failed to load at path: {0}", paths[i]);
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMap::Bind(uint slot /*= 0*/) const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

}