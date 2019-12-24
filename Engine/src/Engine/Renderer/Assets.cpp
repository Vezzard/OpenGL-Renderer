#include "pch.h"
#include "Assets.h"
#include "Renderer.h"

#include <filesystem>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


namespace Engine {

   namespace fs = std::filesystem;

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
      const std::string filename = fs::path(name).filename().string();

      auto& it = m_Data.find(filename);
      if (it != m_Data.end()) {
         return it->second;
      }

      static const std::string texturesPath = "assets/textures/sponza/";
      auto tex = Texture2D::Create(texturesPath + filename);
      m_Data.emplace(filename, tex);

      return tex;
   }


   SPtr<CubeMap> CubemapCreator::Get(const std::string& name)
   {
      auto& it = m_Data.find(name);
      if (it != m_Data.end()) {
         return it->second;
      }

      static const std::string texturesPath = "assets/textures/Cubemaps/";
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


   Engine::SPtr<Engine::Scn::Model> ModelCreator::Get(const std::string& name)
   {
      auto& it = m_Data.find(name);
      if (it != m_Data.end()) {
         return it->second;
      }

      static const std::string modelsPath = "Sandbox/assets/models/";
      const std::string path = projectDir + modelsPath + name;

      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
      ASSERT(scene, "Model loading failed");

      auto model = std::make_shared<Scn::Model>(scene);
      m_Data.emplace(name, model);

      return model;
   }


   Engine::AssetManager& AssetManager::Instance(void)
   {
      static AssetManager instance;
      return instance;
   }

}