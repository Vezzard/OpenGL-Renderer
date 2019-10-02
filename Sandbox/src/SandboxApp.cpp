#include <Engine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLFW/include/GLFW/glfw3.h"

#include <cmath>
#include "Engine/FlyCamera.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Engine/Scene.h"
#include "Engine/Lighting.h"
#include "Engine/Core/Math.h"
#include "Engine/Renderer/Assets.h"

//#include "Glad/include/glad/glad.h"

using namespace Engine;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(float screenWidth, float screenHeight)
		: Layer("Example")
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("D:/Projects/Git/Engine/Sandbox/assets/models/nanosuit/scene.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		ASSERT(scene, "Model loading failed");
		m_Model = std::make_shared<Scn::Model>(scene);
		m_Model->AddTexture("Body", "body_showroom_ddn.png", Scn::Texture::Type::Bump);
		m_Model->AddTexture("Arm", "arm_showroom_ddn.png", Scn::Texture::Type::Bump);
		
      m_Camera.SetPerspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

      AssetManager::GetShader(m_ScreenShader);
      AssetManager::GetShader(m_DefaultShader);
		auto lightShader = AssetManager::GetShader(m_LightSourceShader);
      lightShader->Bind();
		lightShader->UploadUniformFloat3("u_Color", glm::vec3(1.f, 1.f, 1.f));

		m_ScnLight.pointLights.emplace_back();
		m_ScnLight.spotLights.emplace_back();
		
		auto& dl = m_ScnLight.dirLight;
		dl.direction   = glm::vec3(-0.2f, -1.0f, -0.3f);
		dl.ambient		= glm::vec3(0.05f, 0.05f, 0.05f);
		dl.diffuse		= glm::vec3(0.4f, 0.4f, 0.4f);
		dl.specular		= glm::vec3(0.5f, 0.5f, 0.5f);

		auto& pl = m_ScnLight.pointLights[0];
		pl.position		= glm::vec3(6.f, 14.f, 0.f);
		pl.ambient		= glm::vec3(1.f, 1.f, 1.f);
		pl.diffuse		= glm::vec3(0.8f, 0.8f, 0.8f);
		pl.specular		= glm::vec3(1.0f, 1.0f, 1.0f);
		pl.constant		= 1.0f;
		pl.linear		= 0.09f;
		pl.quadratic	= 0.032f;

		auto& sl = m_ScnLight.spotLights[0];
		sl.position = m_Camera.GetPosition();
		Math::matGetForward(m_Camera.GetTransform(), sl.direction);
		sl.ambient		= glm::vec3(0.0f, 0.0f, 0.0f);
		sl.diffuse		= glm::vec3(1.0f, 1.0f, 1.0f);
		sl.specular		= glm::vec3(1.0f, 1.0f, 1.0f);
		sl.constant		= 1.0f;
		sl.linear		= 0.09f;
		sl.quadratic	= 0.032f;
		sl.cutOff		= glm::cos(glm::radians(12.5f));
		sl.outerCutOff	= glm::cos(glm::radians(15.0f));

		for (const auto& l : m_ScnLight.pointLights)
			m_LightSources.emplace_back(std::make_shared<Scn::Cube>(glm::translate(glm::mat4(1.f), l.position), 0.1f));

      m_ScreenFrameBuffer.reset(FrameBuffer::Create());
      m_ScreenFrameBuffer->Bind();

		auto tex2d = Texture2D::Create(nullptr, screenWidth, screenHeight, 3);
		auto tex = m_ScreenQuad->AddTexture(tex2d, Scn::Texture::Type::Diffuse);

      m_ScreenFrameBuffer->AddTexture(tex->GetRenderTex());

      SPtr<RenderBuffer> renderBuffer;
      renderBuffer.reset(RenderBuffer::Create(screenWidth, screenHeight));
      m_ScreenFrameBuffer->AddRenderBuffer(renderBuffer);

      m_ScreenFrameBuffer->Check();
      m_ScreenFrameBuffer->Unbind();
	}

	unsigned int fb;

	void OnUpdate(Timestep ts) override
	{
		m_Camera.Update(ts);

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		RenderCommand::Clear();

		m_LightSources[0]->SetTransform(glm::translate(glm::mat4(1.f), m_ScnLight.pointLights[0].position));

		Renderer::BeginScene(m_Camera.GetRenderCamera());

      m_ScreenFrameBuffer->Bind();
      RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
      RenderCommand::Clear();

		auto& sl = m_ScnLight.spotLights[0];
		sl.position = m_Camera.GetPosition();
		Math::matGetForward(m_Camera.GetTransform(), sl.direction);

      auto defaultShader = AssetManager::GetShader(m_DefaultShader);
		defaultShader->Bind();
		defaultShader->UploadUniformsDefaultLighting(m_ScnLight, m_Camera.GetPosition());
		defaultShader->UploadUniformInt("u_dbgDisableNormalMapping", m_DbgDisableNormalMapping ? 1 : 0);

		m_Model->SetTransform(glm::translate(glm::mat4(1.f), glm::vec3(5.f, 0.f, -4.f)));
		m_Model->Render(defaultShader);

		m_Model->SetTransform(glm::translate(glm::mat4(1.f), glm::vec3(-5.f, 0.f, -4.f)));
		m_Model->Render(defaultShader);

		m_Model->SetTransform(glm::mat4(1.f));
		m_Model->Render(defaultShader);

      auto lightShader = AssetManager::GetShader(m_LightSourceShader);
		for (const auto& ls : m_LightSources)
			ls->Render(lightShader);

      m_ScreenFrameBuffer->Unbind();
      RenderCommand::SetClearColor({ 1.f, 1.f, 1.f, 1.f });
      RenderCommand::Clear();

      auto screenShader = AssetManager::GetShader(m_ScreenShader);
		screenShader->Bind();
		screenShader->UploadUniformFloat("u_dbgPPOffset", m_DbgPPOffset);
		screenShader->UploadUniformInt("u_dbgPPEffect", m_DbgPPEffect);
		m_ScreenQuad->Render(screenShader);

		Renderer::EndScene();
	}

	bool m_DbgDisableNormalMapping = false;
	float m_DbgPPOffset = 0.003f;
	int m_DbgPPEffect = 0;

	virtual void OnImGuiRender() override
	{
		auto& dl = m_ScnLight.dirLight;
		auto& pl = m_ScnLight.pointLights[0];
		auto& sl = m_ScnLight.spotLights[0];

		ImGui::Begin("Settings");
		
		ImGui::ColorEdit3("Dir light direction",	   glm::value_ptr(dl.direction));
		ImGui::ColorEdit3("Dir light ambient",		   glm::value_ptr(dl.ambient));
		ImGui::ColorEdit3("Dir light diffuse",		   glm::value_ptr(dl.diffuse));
		ImGui::ColorEdit3("Dir light specular",		glm::value_ptr(dl.specular));

		ImGui::SliderFloat3("Point light position",	glm::value_ptr(pl.position), -15.f, 15.f);
		ImGui::ColorEdit3("Point light ambient",	   glm::value_ptr(pl.ambient));
		ImGui::ColorEdit3("Point light diffuse",	   glm::value_ptr(pl.diffuse));
		ImGui::ColorEdit3("Point light specular",	   glm::value_ptr(pl.specular));
		ImGui::SliderFloat("Point light constant",	&pl.constant, 0.f, 1.f);
		ImGui::SliderFloat("Point light linear",	   &pl.linear, 0.f, 1.f);
		ImGui::SliderFloat("Point light quadratic",	&pl.quadratic, 0.f, 1.f);

		ImGui::ColorEdit3("Spot light ambient",		glm::value_ptr(sl.ambient));
		ImGui::ColorEdit3("Spot light diffuse",		glm::value_ptr(sl.diffuse));
		ImGui::ColorEdit3("Spot light specular",	   glm::value_ptr(sl.specular));
		ImGui::SliderFloat("Spot light constant",	   &sl.constant, 0.f, 1.f);
		ImGui::SliderFloat("Spot light linear",		&sl.linear, 0.f, 1.f);
		ImGui::SliderFloat("Spot light quadratic",	&sl.quadratic, 0.f, 1.f);

		ImGui::Checkbox("Disable normal mapping",	   &m_DbgDisableNormalMapping);

		ImGui::SliderFloat("Post proc offset",		   &m_DbgPPOffset, 0.f, 0.01f);
		ImGui::SliderInt("Post proc effect",		   &m_DbgPPEffect, 0, 3);

		ImGui::End();
	}

	void OnEvent(Event& event) override
	{
	}
private:
	SceneLight m_ScnLight;
	SPtr<Scn::Model> m_Model;

	std::vector<SPtr<Scn::Cube>> m_LightSources;

   SPtr<Scn::Cube> m_Skybox = std::make_shared<Scn::Cube>(glm::mat4(1.f), 20.f);

	SPtr<Scn::Quad> m_ScreenQuad = std::make_shared<Scn::Quad>(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)));

	std::string m_DefaultShader      = "default";
	std::string m_LightSourceShader  = "flat_color";
	std::string m_ScreenShader       = "default_screen";
   std::string m_ScyboxShader       = "default_skybox";

   SPtr<FrameBuffer> m_ScreenFrameBuffer;

	FlyCamera m_Camera;
};

class Sandbox : public Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight()));
	}

	~Sandbox()
	{

	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}