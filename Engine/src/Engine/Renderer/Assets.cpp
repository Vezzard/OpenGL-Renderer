#include "pch.h"
#include "Assets.h"
#include "Renderer.h"

namespace Engine {

   SPtr<Shader> ShaderCreator::Get(const std::string& name)
   {
      auto& it = m_Data.find(name);
      if (it != m_Data.end()) {
         return it->second;
      }

      static std::string shadersPath = "assets/shaders/";
      std::string ext = GetShaderExt();
      SPtr<Shader> shader;
      shader.reset(Shader::Create(shadersPath + name + "_v" + ext, shadersPath + name + "_f" + ext));
      m_Data.emplace(name, shader);
      return shader;
   }


   std::string ShaderCreator::GetShaderExt(void)
   {
      switch (Renderer::GetAPI())
      {
      case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
      case RendererAPI::API::OpenGL:  return ".glsl";
      }
      CORE_ASSERT(false, "Unknown RendererAPI!");
      return nullptr;
   }


   SPtr<Texture2D> TextureCreator::Get(const std::string& name)
   {
      auto& it = m_Data.find(name);
      if (it != m_Data.end()) {
         return it->second;
      }

      static std::string texturesPath = "assets/textures/";
      auto tex = Texture2D::Create(texturesPath + name);
      m_Data.emplace(name, tex);

      return tex;
   }


   SPtr<CubeMap> CubemapCreator::Get(const std::string& name)
   {
      auto& it = m_Data.find(name);
      if (it != m_Data.end()) {
         return it->second;
      }

      static std::string texturesPath = "assets/textures/Cubemaps/";
      std::string path = texturesPath + name + "/";
      std::vector<std::string> names = {
         path + "posx.jpg",
         path + "negx.jpg",
         path + "posy.jpg",
         path + "negy.jpg",
         path + "posz.jpg",
         path + "negz.jpg",
      };
      auto tex = CubeMap::Create(names);
      m_Data.emplace(name, tex);

      return tex;
   }


   Engine::AssetManager& AssetManager::Instance(void)
   {
      static AssetManager instance;
      return instance;
   }

}