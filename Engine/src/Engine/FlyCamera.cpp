#include "pch.h"
#include "FlyCamera.h"
#include "Input.h"
#include "KeyCodes.h"

namespace Engine {


void FlyCamera::Update(Timestep ts)
{
	static bool moving = false;
	if (!Input::IsMouseButtonPressed(1)) {
		moving = false;
		return;
	}

	static std::pair<float, float> mousePos = { 0.f, 0.f };

	if (!moving) {
		moving = true;
		mousePos = Input::GetMousePosition();
		return;
	}

	static float yaw = -90.f;
	static float pitch = 0.f;

	const auto newMousePos = Input::GetMousePosition();

	yaw   -= m_RotationSpeed * (mousePos.first - newMousePos.first);
	pitch += m_RotationSpeed * (mousePos.second - newMousePos.second);

	mousePos = newMousePos;

	pitch = glm::clamp(pitch, -89.f, 89.f);

	glm::vec3 frw(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	);
	frw = glm::normalize(frw);

	glm::vec3 pos   = GetPosition();
	glm::vec3 unitY = glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 right = glm::normalize(glm::cross(frw, unitY));
	glm::vec3 up    = glm::normalize(glm::cross(right, frw));

	if (Input::IsKeyPressed(KEY_A))
		pos -= right * (m_MoveSpeed * ts);
	else if (Input::IsKeyPressed(KEY_D))
		pos += right * (m_MoveSpeed * ts);

	if (Input::IsKeyPressed(KEY_W))
		pos += frw * (m_MoveSpeed * ts);
	else if (Input::IsKeyPressed(KEY_S))
		pos -= frw * (m_MoveSpeed * ts);

	if (Input::IsKeyPressed(KEY_E))
		pos += up * (m_MoveSpeed * ts);
	else if (Input::IsKeyPressed(KEY_Q))
		pos -= up * (m_MoveSpeed * ts);

	glm::mat4 newTransform = glm::inverse(glm::lookAt(glm::vec3(pos), glm::vec3(pos + frw), unitY));
	SetTransform(newTransform);
}

}
