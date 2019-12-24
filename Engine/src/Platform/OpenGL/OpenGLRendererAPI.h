#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine {

   class OpenGLRendererAPI : public RendererAPI
   {
   public:
      virtual void Init(void) override;
      virtual void SetClearColor(const glm::vec4& color) override;
      virtual void Clear(void) override;
      virtual void DepthMask(bool enable) override;
      virtual void CullFaces(bool enable) override;

      virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
      virtual void Draw(const std::shared_ptr<VertexArray>& vertexArray) override;
   };


}
