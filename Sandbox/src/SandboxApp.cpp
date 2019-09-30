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

#include "Glad/include/glad/glad.h"

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

		m_ScreenShader.reset(Engine::Shader::Create("assets/shaders/default_screen_v.glsl", "assets/shaders/default_screen_f.glsl"));
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


		//@TODO: move to renderer
		//create framebuffer
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);

		// create texture
// 		unsigned int texBuff;
// 		glCreateTextures(GL_TEXTURE_2D, 1, &texBuff);
// 		glTextureStorage2D(texBuff, 1, GL_RGB8, screenWidth, screenHeight);
// 
// 		glTextureParameteri(texBuff, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTextureParameteri(texBuff, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 
// 		glTextureSubImage2D(texBuff, 0, 0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// 		glBindTextureUnit(0, texBuff);

// 		glGenTextures(1, &texColorBuffer);
// 		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 		glBindTexture(GL_TEXTURE_2D, 0);

		auto tex2d = Engine::Texture2D::Create(nullptr, screenWidth, screenHeight, 3);
		auto tex = m_ScreenQuad->AddTexture(tex2d, Engine::Scn::Texture::Type::Diffuse);

		// bind texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->GetRenderTex()->GetRenderId(), 0);

		// create renderbuffer and bind to framebuffer
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		// unbind!
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ASSERT_FAIL("Error");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int fb;

	void OnUpdate(Engine::Timestep ts) override
	{
		m_Camera.Update(ts);

		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Engine::RenderCommand::Clear();

		m_LightSources[0]->SetTransform(glm::translate(glm::mat4(1.f), m_ScnLight.pointLights[0].position));

		Engine::Renderer::BeginScene(m_Camera.GetRenderCamera());
		//@TODO: move to renderer
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		auto& sl = m_ScnLight.spotLights[0];
		sl.position = m_Camera.GetPosition();
		Math::matGetForward(m_Camera.GetTransform(), sl.direction);

		m_DefaultShader->Bind();
		m_DefaultShader->UploadUniformsDefaultLighting(m_ScnLight, m_Camera.GetPosition());
		m_DefaultShader->UploadUniformInt("u_dbgDisableNormalMapping", m_DbgDisableNormalMapping ? 1 : 0);

		m_Model->SetTransform(glm::translate(glm::mat4(1.f), glm::vec3(5.f, 0.f, -4.f)));
		m_Model->Render(m_DefaultShader);

		m_Model->SetTransform(glm::translate(glm::mat4(1.f), glm::vec3(-5.f, 0.f, -4.f)));
		m_Model->Render(m_DefaultShader);

		m_Model->SetTransform(glm::mat4(1.f));
		m_Model->Render(m_DefaultShader);

		for (const auto& ls : m_LightSources)
			ls->Render(m_LightSourceShader);


		//@TODO: move to renderer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_ScreenShader->Bind();
		m_ScreenShader->UploadUniformFloat("u_dbgPPOffset", m_DbgPPOffset);
		m_ScreenShader->UploadUniformInt("u_dbgPPEffect", m_DbgPPEffect);
		m_ScreenQuad->Render(m_ScreenShader);

		Engine::Renderer::EndScene();
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

		ImGui::SliderFloat("Post proc offset",		&m_DbgPPOffset, 0.f, 0.01f);
		ImGui::SliderInt("Post proc effect",		&m_DbgPPEffect, 0, 3);

		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
	}
private:
	Engine::SceneLight m_ScnLight;
	Engine::SPtr<Engine::Scn::Model> m_Model;

	std::vector<Engine::SPtr<Engine::Scn::Cube>> m_LightSources;

	Engine::SPtr<Engine::Scn::Quad> m_ScreenQuad = std::make_shared<Engine::Scn::Quad>(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)));

	Engine::SPtr<Engine::Shader> m_DefaultShader;
	Engine::SPtr<Engine::Shader> m_LightSourceShader;
	Engine::SPtr<Engine::Shader> m_ScreenShader;

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