#include "pch.h"
#include "WindowsInput.h"

#include "Engine/Application.h"
#include <GLFW/glfw3.h>

namespace Engine {

	Input* Input::s_Instance = new WindowsInput();

	static WindowsInput& GetWinInstance(void) 
	{ 
		return *static_cast<WindowsInput*>(Input::GetInstance()); 
	}

	static void ScrollCB(GLFWwindow* window, double x, double y)
	{
		GetWinInstance().m_ScrollState = { x, y };
	}

	void WindowsInput::InitImpl(void)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetScrollCallback(window, ScrollCB);
	}

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	Engine::Input::Position WindowsInput::GetMousePositionImpl(void)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl(void)
	{
		auto[x, y] = GetMousePositionImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl(void)
	{
		auto[x, y] = GetMousePositionImpl();
		return y;
	}

	Engine::Input::Position WindowsInput::GetScrollImpl(void)
	{
		return m_ScrollState;
	}

	void WindowsInput::OnUpdateImpl(void)
	{
		m_ScrollState = { 0.f, 0.f };
	}

}