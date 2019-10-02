#pragma once

#include <glm/glm.hpp>

namespace Engine {

	class Camera
	{
	public:
		Camera() {}

		void				   SetOrthographic			(float left, float right, float bottom, float top, const glm::mat4& transform);
		void				   SetPerspective			   (float fovy, float aspect, float zNear, float zFar, const glm::mat4& transform);

		const glm::mat4&	GetProjectionMatrix		(void) const { return m_ProjectionMatrix; }
		const glm::mat4&	GetViewMatrix			   (void) const { return m_ViewMatrix; }
		const glm::mat4&	GetViewProjectionMatrix	(void) const { return m_ViewProjectionMatrix; }
		void				   RecalculateViewMatrix	(const glm::mat4& transform);

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	};

}
