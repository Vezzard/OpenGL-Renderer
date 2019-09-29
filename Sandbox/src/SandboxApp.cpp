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

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer(float screenWidth, float screenHeight)
		: Layer("Example")
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("D:/Projects/Engine/versions/1.3/Sandbox/assets/models/nanosuit/scene.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		ASSERT(scene);
		m_Model = std::make_shared<Engine::Scn::Model>(scene);
		m_Model->AddTexture("Body", "body_showroom_ddn.png", Engine::Scn::Texture::Type::Bump);
		m_Model->AddTexture("Arm", "arm_showroom_ddn.png", Engine::Scn::Texture::Type::Bump);
		
		m_Camera.SetPerspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

		m_DefaultShader.reset(Engine::Shader::Create("assets/shaders/default_v.glsl", "assets/shaders/default_f.glsl"));
		m_LightSourceShader.reset(Engine::Shader::Create("assets/shaders/flat_color_v.glsl", "assets/shaders/flat_color_f.glsl"));
		m_LightSourceShader->Bind();
		m_LightSourceShader->UploadUniformFloat3("u_Color", glm::vec3(1.f, 1.f, 1.f));

		m_ScnLight.pointLights.emplace_back();
		m_ScnLight.spotLights.emplace_back();
		
		auto& dl = m_ScnLight.dirLight;
		dl.direction    = glm::vec3(-0.2f, -1.0f, -0.3f);
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
			m_LightSources.emplace_back(std::make_shared<Engine::Scn::Cube>(glm::translate(glm::mat4(1.f), l.position), 0.1f));
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_Camera.Update(ts);

		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Engine::RenderCommand::Clear();

		m_LightSources[0]->SetTransform(glm::translate(glm::mat4(1.f), m_ScnLight.pointLights[0].position));

		Engine::Renderer::BeginScene(m_Camera.GetRenderCamera());

		auto& sl = m_ScnLight.spotLights[0];
		sl.position = m_Camera.GetPosition();
		Math::matGetForward(m_Camera.GetTransform(), sl.direction);

		m_DefaultShader->Bind();
		m_DefaultShader->UploadUniformsDefaultLighting(m_ScnLight, m_Camera.GetPosition());
		m_DefaultShader->UploadUniformInt("u_dbgDisableNormalMapping", m_DbgDisableNormalMapping ? 1 : 0);
		m_Model->Render(m_DefaultShader);

		for (const auto& ls : m_LightSources)
			ls->Render(m_LightSourceShader);

		Engine::Renderer::EndScene();
	}

	bool m_DbgDisableNormalMapping = false;

	virtual void OnImGuiRender() override
	{
		auto& dl = m_ScnLight.dirLight;
		auto& pl = m_ScnLight.pointLights[0];
		auto& sl = m_ScnLight.spotLights[0];

		ImGui::Begin("Settings");
		
		ImGui::ColorEdit3("Dir light direction",	glm::value_ptr(dl.direction));
		ImGui::ColorEdit3("Dir light ambient",		glm::value_ptr(dl.ambient));
		ImGui::ColorEdit3("Dir light diffuse",		glm::value_ptr(dl.diffuse));
		ImGui::ColorEdit3("Dir light specular",		glm::value_ptr(dl.specular));

		ImGui::SliderFloat3("Point light position",	glm::value_ptr(pl.position), -15.f, 15.f);
		ImGui::ColorEdit3("Point light ambient",	glm::value_ptr(pl.ambient));
		ImGui::ColorEdit3("Point light diffuse",	glm::value_ptr(pl.diffuse));
		ImGui::ColorEdit3("Point light specular",	glm::value_ptr(pl.specular));
		ImGui::SliderFloat("Point light constant",	&pl.constant, 0.f, 1.f);
		ImGui::SliderFloat("Point light linear",	&pl.linear, 0.f, 1.f);
		ImGui::SliderFloat("Point light quadratic",	&pl.quadratic, 0.f, 1.f);

		ImGui::ColorEdit3("Spot light ambient",		glm::value_ptr(sl.ambient));
		ImGui::ColorEdit3("Spot light diffuse",		glm::value_ptr(sl.diffuse));
		ImGui::ColorEdit3("Spot light specular",	glm::value_ptr(sl.specular));
		ImGui::SliderFloat("Spot light constant",	&sl.constant, 0.f, 1.f);
		ImGui::SliderFloat("Spot light linear",		&sl.linear, 0.f, 1.f);
		ImGui::SliderFloat("Spot light quadratic",	&sl.quadratic, 0.f, 1.f);

		ImGui::Checkbox("Disable normal mapping",	&m_DbgDisableNormalMapping);

		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
	}
private:
	Engine::SceneLight m_ScnLight;
	Engine::SPtr<Engine::Scn::Model> m_Model;

	std::vector<Engine::SPtr<Engine::Scn::Cube>> m_LightSources;

	Engine::SPtr<Engine::Shader> m_DefaultShader;
	Engine::SPtr<Engine::Shader> m_LightSourceShader;

	Engine::FlyCamera m_Camera;
};

class Sandbox : public Engine::Application
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