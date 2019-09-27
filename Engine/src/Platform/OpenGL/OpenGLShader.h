#pragma once

#include "Engine/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Engine {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniformInt(const std::string& name, int value) override;

		void UploadUniformFloat(const std::string& name, float value) override;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value) override;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value) override;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value) override;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) override;
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
	private:
		uint32_t m_RendererID;
	};

}