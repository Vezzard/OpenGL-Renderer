#include "pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	void Camera::SetOrthographic(float left, float right, float bottom, float top, const glm::mat4& transform)
	{
		m_ProjectionMatrix		= glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewMatrix			= glm::inverse(transform);
		m_ViewProjectionMatrix	= m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetPerspective(float fovy, float aspect, float zNear, float zFar, const glm::mat4& transform)
	{
		m_ProjectionMatrix		= glm::perspective(fovy, aspect, zNear, zFar);
		m_ViewMatrix			= glm::inverse(transform);
		m_ViewProjectionMatrix	= m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::RecalculateViewMatrix(const glm::mat4& transform)
	{
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}