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
class Vertex;
struct Face;
class Light;
//class Animation;
//class CameraSpot;

class Mesh
{
public:
	Mesh(const aiMesh* mesh, const glm::mat4& transform, const glm::mat4& parentTransform, const SPtr<Material>& material);

	void SetParentTransform(const glm::mat4& transform) { m_WorldTransform = transform * m_LocalTransform; }

	void Render(void);
	void Render(const SPtr<Engine::Shader>& shader);
	
	void GetIndecies(std::vector<uint>& indicies) const;

private:
	void SetupRenderable(void);

	std::string			m_Name;
	std::vector<Vertex> m_Verts;
	std::vector<Face>	m_Faces;
	glm::mat4			m_WorldTransform;
	glm::mat4			m_LocalTransform;
	SPtr<Material>		m_Material;
	SPtr<VertexArray>	m_VAO;
};


class Model
{
public:
	Model(const aiScene* scene);

	SPtr<Texture> GetTexture(uint idx)						{ return m_Textures[idx]; }
	void	      Render    (void)							{ for (auto& m : m_Meshes) m->Render(); }
	void	      Render	(const SPtr<Shader>& shader)	{ for (auto& m : m_Meshes) m->Render(shader); }

private:
	void ProcessNode(const aiNode* node, const aiScene* scene);

	glm::mat4 m_Transform = glm::mat4(1.f);
	
	std::vector<SPtr<Mesh>>		 m_Meshes;
	std::vector<SPtr<Material>>  m_Materials;
	std::vector<SPtr<Texture>>   m_Textures;
};


class Texture
{
public:
	enum class Type {
		None,
		Ambient,
		Diffuse,
		Specular
	};

	Texture(const std::string& name)
		: m_Name(name) { }

	static aiTextureType	ConvertType		(Type type);
	bool					IsLoaded		(void) const	{ return m_RenderTex.get(); }
	void					Load			(void);
	const std::string&		GetName			(void) const	{ return m_Name; }
	void					SetType			(Type type)		{ m_Type = type; }
	Type					GetType			(void) const	{ return m_Type; }
	SPtr<Texture2D>			GetRenderTex	(void)			{ return m_RenderTex; }

private:
	Type m_Type = Type::None;
	std::string m_Name;

	SPtr<Texture2D> m_RenderTex;
};


class Material
{
public:
	Material(const aiMaterial* material, Model& model, const std::string& shader = "default");

	const std::vector<SPtr<Texture>>& GetTextures(void) const { return m_Textures; } 

private:
	void LoadTextures(const aiMaterial* material, Model& model, Texture::Type type);

	std::string m_Shader;
	std::vector<SPtr<Texture>> m_Textures;
};


class Vertex
{
public:
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec2& uv)
		: m_Position(position)
		, m_Normal(normal)
		, m_Tangent(tangent)
		, m_Bitangent(bitangent)
		, m_UV(uv) { }


private:
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec3 m_Tangent;
	glm::vec3 m_Bitangent;
	glm::vec2 m_UV;
};


struct Face
{
	std::vector<uint> indecies;
};

}
}
