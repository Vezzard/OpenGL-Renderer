#pragma once

#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include "../Scene.h"

namespace Engine {


   template<class T>
   class AssetCreator
   {
   public:
      virtual SPtr<T> Get(const std::string& name) = 0;

   protected:
      std::unordered_map<std::string, SPtr<T>> m_Data;
   };


   class ShaderCreator : public AssetCreator<Shader>
   {
   public:
      virtual SPtr<Shader> Get(const std::string& name) override;

   private:
      std::string GetShaderExt(void);
   };


   class TextureCreator : public AssetCreator<Texture2D>
   {
   public:
      virtual SPtr<Texture2D> Get(const std::string& name) override;
   };


   class CubemapCreator : public AssetCreator<CubeMap>
   {
   public:
      virtual SPtr<CubeMap> Get(const std::string& name) override;
   };


   class ModelCreator : public AssetCreator<Scn::Model>
   {
   public:
      virtual SPtr<Scn::Model> Get(const std::string& name) override;
   };


   class AssetManager
   {
   private:
      AssetManager(void) = default;

   public:
      static SPtr<Shader>      GetShader(const std::string& name) { return Instance().m_ShaderCreator.Get(name); }
      static SPtr<Texture2D>   GetTexture2D(const std::string& name) { return Instance().m_TextureCreator.Get(name); }
      static SPtr<CubeMap>     GetCubemap(const std::string& name) { return Instance().m_CubemapCreator.Get(name); }
      static SPtr<Scn::Model>  GetModel(const std::string& name) { return Instance().m_ModelCreator.Get(name); }

   private:
      static AssetManager& Instance(void);

      ShaderCreator  m_ShaderCreator;
      TextureCreator m_TextureCreator;
      CubemapCreator m_CubemapCreator;
      ModelCreator   m_ModelCreator;
   };

}
