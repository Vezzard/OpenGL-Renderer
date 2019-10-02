#include "pch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Assets.h"

namespace Engine {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(const SPtr<Camera>&  camera)
	{
		s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
      s_SceneData->view = camera->GetViewMatrix();
      s_SceneData->projection = camera->GetProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const SPtr<Shader>& shader, const SPtr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
 		shader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

      shader->UploadUniformMat4("u_projection", s_SceneData->projection);
      shader->UploadUniformMat4("u_view", s_SceneData->view);

		vertexArray->Bind();

      //if (shader == AssetManager::GetShader("default_skybox"))
      //   RenderCommand::Draw(vertexArray);
      //else
		   RenderCommand::DrawIndexed(vertexArray);
	}

}