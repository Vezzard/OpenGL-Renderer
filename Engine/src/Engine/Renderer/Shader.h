#pragma once

#include <string>
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"
#include "../Lighting.h"

namespace Engine {

   class Shader
   {
   public:
      virtual ~Shader() = default;

      virtual void Bind(void) const = 0;
      virtual void Unbind(void) const = 0;

      virtual void UploadUniformInt(const std::string& name, int value) = 0;

      virtual void UploadUniformFloat(const std::string& name, float value) = 0;
      virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
      virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
      virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

      virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) = 0;
      virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;

      virtual void UploadUniformsDefaultLighting(const SceneLight& scnLight, const glm::vec3& cameraPos);


      static Shader* Create(const std::string& vertexPath, const std::string& fragmentPath);

   private:
      virtual void UploadUniformLight(const std::string& name, const Light& light);
   };

}