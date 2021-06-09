#include <Engine.h>

using namespace Engine;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(float screenWidth, float screenHeight)
		: Layer("Example")
	{
	}

	void OnUpdate(Timestep ts) override
	{
	}

	void OnImGuiRender() override
	{
	}

	void OnEvent(Event& event) override
	{
	}
};

class Sandbox : public Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight()));
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}