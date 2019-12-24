#include "pch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "FileReader.h"
#include "Scene.h"

namespace Engine {


   void Shader::UploadUniformsDefaultLighting(const SceneLight& scnLight, const glm::vec3& cameraPos)
   {
      UploadUniformFloat3("u_viewPos", cameraPos);

      UploadUniformFloat3("u_dirLight.direction", scnLight.dirLight.direction);
      UploadUniformLight("u_dirLight", scnLight.dirLight);

      for (uint i = 0; i < scnLight.pointLights.size(); ++i) {
         char buff[32];
         std::snprintf(buff, sizeof(buff), "u_pointLights[%d]", i);
         std::string strName = buff;
         auto& light = scnLight.pointLights[i];
         UploadUniformLight(strName, light);
         UploadUniformFloat3(strName + ".position", light.position);
         UploadUniformFloat(strName + ".constant", light.constant);
         UploadUniformFloat(strName + ".linear", light.linear);
         UploadUniformFloat(strName + ".quadratic", light.quadratic);
      }

      for (uint i = 0; i < scnLight.spotLights.size(); ++i) {
         char buff[32];
         std::snprintf(buff, sizeof(buff), "u_spotLights[%d]", i);
         std::string strName = buff;
         auto& light = scnLight.spotLights[i];
         UploadUniformLight(strName, light);
         UploadUniformFloat3(strName + ".position", light.position);
         UploadUniformFloat3(strName + ".direction", light.direction);
         UploadUniformFloat(strName + ".constant", light.constant);
         UploadUniformFloat(strName + ".linear", light.linear);
         UploadUniformFloat(strName + ".quadratic", light.quadratic);
         UploadUniformFloat(strName + ".cutOff", light.cutOff);
         UploadUniformFloat(strName + ".outerCutOff", light.outerCutOff);
      }
   }


   void Shader::UploadUniformLight(const std::string& name, const Light& light)
   {
      UploadUniformFloat3(name + ".ambient", light.ambient);
      UploadUniformFloat3(name + ".diffuse", light.diffuse);
      UploadUniformFloat3(name + ".specular", light.specular);
   }


   Shader* Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
   {
      std::string vsrc, fsrc;
      FileReader vfile(vertexPath), ffile(fragmentPath);
      if (!vfile.Parse(vsrc)) {
         ASSERT_FAIL("Shader read failed");
      }
      if (!ffile.Parse(fsrc)) {
         ASSERT_FAIL("Shader read failed");
      }


      switch (Renderer::GetAPI())
      {
      case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
      case RendererAPI::API::OpenGL:  return new OpenGLShader(vsrc, fsrc);
      }

      CORE_ASSERT(false, "Unknown RendererAPI!");
      return nullptr;
   }

}