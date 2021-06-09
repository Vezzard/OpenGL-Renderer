#include <Engine.h>
#include <vulkan/vulkan_core.h>

#include "GLFW/glfw3.h"

using namespace Engine;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(float screenWidth, float screenHeight)
		: Layer("Example")
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		CORE_ASSERT(result == VK_SUCCESS, "vkCreateInstance error");
		INFO_CORE("vkCreateInstance result: {0}", result);

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		INFO_CORE("available extensions:");

		for (const auto& extension : extensions) 
		{
			INFO_CORE("\t{0}", extension.extensionName);
		}
	}

	~ExampleLayer() override
	{
		vkDestroyInstance(instance, nullptr);
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

private:
	VkInstance instance;
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