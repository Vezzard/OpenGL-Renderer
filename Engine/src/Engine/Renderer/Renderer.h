#pragma once

#include "RenderCommand.h"

#include "Camera.h"
#include "Shader.h"

namespace Engine {

   class Renderer
   {
   public:
      static void Init();

      static void BeginScene(const SPtr<Camera>& camera);
      static void EndScene();

      static void Submit(const SPtr<Shader>& shader, const SPtr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

      inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
   private:
      struct SceneData
      {
         glm::mat4 ViewProjectionMatrix;
         glm::mat4 view;
         glm::mat4 projection;
      };

      static SceneData* s_SceneData;
   };

   class ShaderMng
   {

   };
}
