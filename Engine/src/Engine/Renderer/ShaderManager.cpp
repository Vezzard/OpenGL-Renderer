#include "pch.h"
#include "ShaderManager.h"
#include "Renderer.h"

namespace Engine {

   Engine::SPtr<Shader> ShaderManager::Get(const std::string& name)
   {
      static ShaderManager instance;
      
      auto& it = instance.m_Shaders.find(name);
      if (it != instance.m_Shaders.end()) {
         return it->second;
      }

      static std::string shadersPath = "assets/shaders/";
      std::string ext = instance.GetShaderExt();
      SPtr<Shader> shader;
      shader.reset(Shader::Create(shadersPath + name + "_v" + ext, shadersPath + name + "_f" + ext));
      instance.m_Shaders.emplace(name, shader);
      return shader;
   }


   std::string ShaderManager::GetShaderExt(void)
   {
      switch (Renderer::GetAPI())
      {
      case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
      case RendererAPI::API::OpenGL:  return ".glsl";
      }
      CORE_ASSERT(false, "Unknown RendererAPI!");
      return nullptr;
   }

}