#pragma once

#include "pch.h"
#include "Shader.h"


namespace Engine {

   class ShaderManager
   {
      ShaderManager(void) = default;
      ShaderManager(const ShaderManager&);

   public:
      static SPtr<Shader> Get(const std::string& name);
   private:
      std::string GetShaderExt(void);

      std::unordered_map<std::string, SPtr<Shader>> m_Shaders;
   };

}
