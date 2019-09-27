#pragma once

#include "glm/detail/type_vec3.hpp"

namespace Engine {

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct DirectionalLight : Light
{
	glm::vec3 direction;
};

struct PointLight : Light
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight : Light
{
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};

struct SceneLight
{
	std::vector<PointLight>	pointLights;
	std::vector<SpotLight>	spotLights;
	DirectionalLight		dirLight;
};


}

