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
	Engine::SPtr<Engine::Scn::Model> m_Model;

public:
	ExampleLayer(float screenWidth, float screenHeight)
		: Layer("Example")
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("D:/Projects/Engine/versions/1.3/Sandbox/assets/models/nanosuit/scene.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		ASSERT(scene);
		m_Model = std::make_shared<Engine::Scn::Model>(scene);
		
		m_Camera.SetPerspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

		m_DefaultShader.reset(Engine::Shader::Create("assets/shaders/default_v.glsl", "assets/shaders/default_f.glsl"));

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


		m_SquareVA.reset(Engine::VertexArray::Create());
		m_LightSourceVA.reset(Engine::VertexArray::Create());

		float lsVertices[6 * 6 * 3] = {
			// position			  
		   -0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f,  0.1f, -0.1f,
			0.1f,  0.1f, -0.1f,
		   -0.1f,  0.1f, -0.1f,
		   -0.1f, -0.1f, -0.1f,

		   -0.1f, -0.1f,  0.1f,
			0.1f, -0.1f,  0.1f,
			0.1f,  0.1f,  0.1f,
			0.1f,  0.1f,  0.1f,
	       -0.1f,  0.1f,  0.1f,
		   -0.1f, -0.1f,  0.1f,

		   -0.1f,  0.1f,  0.1f,
		   -0.1f,  0.1f, -0.1f,
		   -0.1f, -0.1f, -0.1f,
		   -0.1f, -0.1f, -0.1f,
		   -0.1f, -0.1f,  0.1f,
		   -0.1f,  0.1f,  0.1f,

			0.1f,  0.1f,  0.1f,
			0.1f,  0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f,  0.1f,
			0.1f,  0.1f,  0.1f,

		   -0.1f, -0.1f, -0.1f,
			0.1f, -0.1f, -0.1f,
			0.1f, -0.1f,  0.1f,
			0.1f, -0.1f,  0.1f,
		   -0.1f, -0.1f,  0.1f,
		   -0.1f, -0.1f, -0.1f,

		   -0.1f,  0.1f, -0.1f,
			0.1f,  0.1f, -0.1f,
			0.1f,  0.1f,  0.1f,
			0.1f,  0.1f,  0.1f,
		   -0.1f,  0.1f,  0.1f,
		   -0.1f,  0.1f, -0.1f
		};

		const glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);

		Engine::SPtr<Engine::VertexBuffer> lsVB;
		lsVB.reset(Engine::VertexBuffer::Create(lsVertices, sizeof(lsVertices)));
		lsVB->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
		});
		m_LightSourceVA->AddVertexBuffer(lsVB);
		m_LightSourceShader.reset(Engine::Shader::Create("assets/shaders/simple_v.glsl", "assets/shaders/simple_f.glsl"));
		m_LightSourceShader->Bind();
		m_LightSourceShader->UploadUniformFloat3("u_Color", lightColor);

		float squareVertices[6 * 6 * 8] = {
			// position			  // texture   // normal
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f
		};

		Engine::SPtr<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" },
			{ Engine::ShaderDataType::Float3, "a_Normal" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Engine::SPtr<Engine::IndexBuffer> squareIB;
		squareIB.reset(Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_TextureShader.reset(Engine::Shader::Create("assets/shaders/simple_texture_v.glsl", "assets/shaders/simple_texture_f.glsl"));

		//m_Texture  = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_Texture  = Engine::Texture2D::Create("D:/Projects/Engine/assets/models/nanosuit/textures/helmet_dif.png");
		m_Texture2 = Engine::Texture2D::Create("assets/textures/metal-texture-rusty.jpg");

		GlobalTexture = m_Texture;

		m_TextureShader->Bind();
		m_TextureShader->UploadUniformFloat3("u_lightColor", lightColor);
		m_TextureShader->UploadUniformFloat3("u_lightPos", glm::vec3(
			m_LightSourceTransform[3].x,
			m_LightSourceTransform[3].y,
			m_LightSourceTransform[3].z));

		GlobalTexture = m_Texture;
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_Camera.Update(ts);

		
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_Camera.GetRenderCamera());
		
		float deg = (float)fmod(glfwGetTime() * 15, 360);
		m_CubeTransform =
			glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -5.f)) *
  			glm::rotate(glm::mat4(1.f), glm::radians(deg), glm::vec3(0.f, 1.f, 0.f));
//  			glm::rotate(glm::mat4(1.f), (float)glm::radians(cos(glfwGetTime()) * 360 / 10), glm::vec3(1.f, 0.f, 0.f));

		m_TextureShader->UploadUniformMat4("u_Transform", m_CubeTransform);
		m_TextureShader->UploadUniformMat4("u_ViewProjection", m_Camera.GetRenderCamera()->GetViewProjectionMatrix());
		m_TextureShader->UploadUniformFloat3("u_viewPos", glm::vec3(m_Camera.GetPosition()));

		m_TextureShader->UploadUniformInt("u_Texture0", 0);
		m_TextureShader->UploadUniformInt("u_Texture1", 1);
		//m_Texture->Bind(0);
		//m_Texture2->Bind(1);
		//Engine::Renderer::Submit(m_TextureShader, m_SquareVA, m_CubeTransform);
		//Engine::Renderer::Submit(m_LightSourceShader, m_LightSourceVA, m_LightSourceTransform);

		auto& sl = m_ScnLight.spotLights[0];
		sl.position = m_Camera.GetPosition();
		Math::matGetForward(m_Camera.GetTransform(), sl.direction);

		float intensity = (1 - sl.outerCutOff) / (sl.cutOff - sl.outerCutOff);

		m_DefaultShader->UploadUniformsDefaultLighting(m_ScnLight, m_Camera.GetPosition());
		m_Model->Render(m_DefaultShader);

		Engine::Renderer::EndScene();
	}

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
		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
	}
private:
	Engine::SceneLight m_ScnLight;

	Engine::SPtr<Engine::Shader> m_DefaultShader;
	Engine::SPtr<Engine::Shader> m_TextureShader;
	Engine::SPtr<Engine::VertexArray> m_SquareVA;
	glm::mat4 m_CubeTransform = glm::mat4(1.f);

	Engine::SPtr<Engine::Shader> m_LightSourceShader;
	Engine::SPtr<Engine::VertexArray> m_LightSourceVA;
	glm::mat4 m_LightSourceTransform = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 10.f, -5.f));

	Engine::SPtr<Engine::Texture2D> m_Texture, m_Texture2;

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