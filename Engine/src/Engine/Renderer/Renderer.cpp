#include "pch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(const SPtr<Camera>&  camera)
	{
		s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const SPtr<Shader>& shader, const SPtr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
 		shader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		//RenderCommand::Draw(vertexArray);
	}

}