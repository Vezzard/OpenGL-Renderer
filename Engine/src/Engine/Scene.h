#pragma once

#include "pch.h"
#include "assimp/scene.h"
#include "glm/gtx/mixed_product.hpp"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"


extern Engine::SPtr<Engine::Texture2D> GlobalTexture;

namespace Engine {
namespace Scn {

class ModelPart;
class Mesh;
class Material;
class Texture;
struct Vertex;
struct Face;
class Light;
//class Animation;
//class CameraSpot;

class Mesh
{
public:
	Mesh(void) {};
	Mesh(const aiMesh* mesh, const glm::mat4& transform, const glm::mat4& parentTransform, const SPtr<Material>& material);

	void SetParentTransform(const glm::mat4& transform) { m_WorldTransform = transform * m_LocalTransform; }

	void Render(void) const;
	void Render(const SPtr<Engine::Shader>& shader) const;
	
	void GetIndecies(std::vector<uint>& indicies) const;

protected:
	virtual BufferLayout	GetVboLayout	(void) const;
	virtual void			UploadUniforms	(const SPtr<Shader>& shader) const;
	void					SetupRenderable	(void);
	
	glm::mat4 m_WorldTransform = glm::mat4(1.f);
	std::vector<Vertex> m_Verts;
	std::vector<Face>	m_Faces;

private:

	std::string			m_Name;
	glm::mat4			m_LocalTransform = glm::mat4(1.f);
	SPtr<Material>		m_Material;
	SPtr<VertexArray>	m_VAO;
};


class Texture
{
public:
	enum class Type {
		None,
		Ambient,
		Diffuse,
		Specular,
		Normal,
		Bump,
	};

	Texture(const std::string& name)
		: m_Name(name) { }

	static aiTextureType	ConvertType(Type type);
	bool					IsLoaded(void) const { return m_RenderTex.get(); }
	void					Load(void);
	const std::string&		GetName(void) const { return m_Name; }
	void					SetType(Type type) { m_Type = type; }
	Type					GetType(void) const { return m_Type; }
	SPtr<Texture2D>			GetRenderTex(void) { return m_RenderTex; }

private:
	Type m_Type = Type::None;
	std::string m_Name;

	SPtr<Texture2D> m_RenderTex;
};


class Model
{
public:
	Model(const aiScene* scene);

	SPtr<Texture> GetTexture	(uint idx)						{ return m_Textures[idx]; }
	void	      Render		(void)							{ for (auto& m : m_Meshes) m->Render(); }
	void	      Render		(const SPtr<Shader>& shader)	{ for (auto& m : m_Meshes) m->Render(shader); }

	SPtr<Material>	GetMaterial	(const std::string& name);
	void			AddTexture	(const std::string& matName, const std::string& texName, Texture::Type type);

private:
	void ProcessNode(const aiNode* node, const aiScene* scene);

	glm::mat4 m_Transform = glm::mat4(1.f);
	
	std::vector<SPtr<Mesh>>		 m_Meshes;
	std::vector<SPtr<Material>>  m_Materials;
	std::vector<SPtr<Texture>>   m_Textures;
};


class Material
{
public:
	using TextureList = std::vector<SPtr<Texture>>;

	Material(const aiMaterial* material, Model& model, const std::string& shader = "default");

	const TextureList& GetTextures	(void) const { return m_Textures; }
	const std::string& GetName		(void) const { return m_Name; }	
	
	void AddTexture(const SPtr<Texture>& tex) { m_Textures.emplace_back(tex); }

private:
	void LoadTextures(const aiMaterial* material, Model& model, Texture::Type type);

	std::string m_Shader;
	std::string m_Name;
	TextureList m_Textures;
};


struct Vertex
{
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec2& uv)
		: position(position)
		, normal(normal)
		, tangent(tangent)
		, bitangent(bitangent)
		, uv(uv) { }

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 uv;
};


struct Face
{
	Face(void) {};
	Face(uint i0, uint i1, uint i2) { indecies = { i0, i1, i2 }; };

	std::vector<uint> indecies;
};


class Cube : public Mesh
{
public:
	Cube(const glm::mat4& transform, float scale = 1.f);

	void SetTransform(const glm::mat4& transform) { m_WorldTransform = transform; } 

protected:
	virtual void UploadUniforms(const SPtr<Shader>& shader) const override {}
};


}
}
