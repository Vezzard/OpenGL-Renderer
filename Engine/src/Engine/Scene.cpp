#include "pch.h"
#include "Scene.h"
#include "Renderer/Renderer.h"

namespace Engine {
namespace Scn {


static void VecAssimpToGlm(const aiVector3D& aVec, glm::vec3& gVec)
{
	for (uint i = 0; i < 3; ++i)	{
		gVec[i] = aVec[i];
	}
}


static void MatAssimpToGlm(const aiMatrix4x4& aMat, glm::mat4& gMat)
{
	for (uint i = 0; i < 4; ++i) {
		for (uint j = 0; j < 4; ++j)
			gMat[i][j] = aMat[i][j];
	}
}


Model::Model(const aiScene* scene)
{
	m_Textures.reserve(scene->mNumTextures);
	for (uint i = 0; i < scene->mNumTextures; ++i) {
		aiTexture* aiTex = scene->mTextures[i];
		auto& t = m_Textures.emplace_back(std::make_shared<Texture>(aiTex->mName));
		t->Load();
	}

	m_Materials.reserve(scene->mNumMaterials);
	for (uint i = 0; i < scene->mNumMaterials; ++i) {
		m_Materials.emplace_back(std::make_shared<Material>(scene->mMaterials[i], *this));
	}

	m_Meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);
}


SPtr<Material> Model::GetMaterial(const std::string& name)
{
	for (auto& m : m_Materials) {
		if (m->GetName() == name) {
			return m;
		}
	}
	ASSERT_FAIL("No material {0}", name);
	return nullptr;
}


SPtr<Texture> Model::AddTexture(const std::string& matName, const std::string& texName, Texture::Type type)
{
	auto tex = std::make_shared<Engine::Scn::Texture>(texName);
	tex->Load();
	return AddTexture(matName, tex, type);
}


SPtr<Texture> Model::AddTexture(const std::string& matName, const SPtr<Texture>& tex, Texture::Type type)
{
	tex->SetType(type);
	GetMaterial(matName)->AddTexture(tex);
	return tex;
}

void Model::SetTransform(const glm::mat4& transform)
{
	m_Transform = transform;
	for (auto& m : m_Meshes)
		m->SetParentTransform(transform);
}


void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{
	glm::mat4 transform;
	MatAssimpToGlm(node->mTransformation, transform);
	for (uint i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.emplace_back(std::make_shared<Mesh>(mesh, transform, m_Transform, m_Materials[mesh->mMaterialIndex]));
	}

	for (uint i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(node->mChildren[i], scene);
	}
}


Mesh::Mesh(const aiMesh* mesh, const glm::mat4& transform, const glm::mat4& parentTransform, const SPtr<Material>& material)
	: m_LocalTransform(transform)
	, m_Name(mesh->mName.C_Str())
	, m_Material(material)
{
	SetParentTransform(parentTransform);

	m_Verts.reserve(mesh->mNumVertices);
	m_Faces.reserve(mesh->mNumFaces);
	for (uint i = 0; i < mesh->mNumVertices; ++i) {
		glm::vec3 position, normal, tangent, bitangent;
		glm::vec2 uv;
		VecAssimpToGlm(mesh->mVertices[i],	 position);
		VecAssimpToGlm(mesh->mNormals[i],	 normal);
		VecAssimpToGlm(mesh->mTangents[i],	 tangent);
		VecAssimpToGlm(mesh->mBitangents[i], bitangent);
		if (mesh->mTextureCoords[0]) {
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
		}
		m_Verts.emplace_back(position, normal, tangent, bitangent, uv);
	}
	for (uint i = 0; i < mesh->mNumFaces; ++i) {
		Face& f = m_Faces.emplace_back();
		aiFace& aiF = mesh->mFaces[i];
		f.indecies.reserve(aiF.mNumIndices);
		for (uint j = 0; j < aiF.mNumIndices; ++j) {
			f.indecies.emplace_back(aiF.mIndices[j]);
		}
	}

	SetupRenderable();
}


void Mesh::SetupRenderable(void)
{
	m_VAO.reset(VertexArray::Create());
	
	SPtr<VertexBuffer> vbo;
	float* rawVerts = reinterpret_cast<float*>(m_Verts.data());
	vbo.reset(VertexBuffer::Create(rawVerts, m_Verts.size() * sizeof(Vertex)));
	vbo->SetLayout(GetVboLayout());
	m_VAO->AddVertexBuffer(vbo);

	std::vector<uint> inds;
	GetIndecies(inds);
	SPtr<IndexBuffer> ebo;
	ebo.reset(Engine::IndexBuffer::Create(inds.data(), inds.size()));
	m_VAO->SetIndexBuffer(ebo);
}


Engine::BufferLayout Mesh::GetVboLayout(void) const
{
	return {
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float3, "a_Normal" },
		{ Engine::ShaderDataType::Float3, "a_Tangent" },
		{ Engine::ShaderDataType::Float3, "a_Bitangent" },
		{ Engine::ShaderDataType::Float2, "a_UV" }
	};
}


void Mesh::Render(void) const
{
	
}


void Mesh::Render(const SPtr<Shader>& shader) const
{
	shader->Bind();
	UploadUniforms(shader);
	Renderer::Submit(shader, m_VAO, m_WorldTransform);
}


void Mesh::UploadUniforms(const SPtr<Shader>& shader) const
{
	shader->UploadUniformFloat("u_material.shininess", 32.f);

	int normalMapping = 0;
	for (const auto& t : m_Material->GetTextures()) {
		switch (t->GetType())
		{
		case Texture::Type::Diffuse:
			shader->UploadUniformInt("u_material.diffuse", 0);
			t->GetRenderTex()->Bind(0);
			break;
		case Texture::Type::Specular:
			shader->UploadUniformInt("u_material.specular", 1);
			t->GetRenderTex()->Bind(1);
			break;
		case Texture::Type::Normal:
			shader->UploadUniformInt("u_material.normal", 2);
			t->GetRenderTex()->Bind(2);
			normalMapping = 1;
			break;
		case Texture::Type::Bump:
			shader->UploadUniformInt("u_material.normal", 2);
			t->GetRenderTex()->Bind(2);
			normalMapping = 1;
			break;
		}
	}
	shader->UploadUniformInt("u_normalMapping", normalMapping);
}


void Mesh::GetIndecies(std::vector<uint>& indicies) const
{
	if (m_Faces.empty())
		return;

	indicies.reserve(m_Faces.size() * m_Faces[0].indecies.size());
	for (const Face& f : m_Faces) {
		indicies.insert(indicies.end(), f.indecies.begin(), f.indecies.end());
	}
}


Engine::SPtr<Texture> Mesh::AddTexture(const SPtr<Texture2D>& tex, Texture::Type type)
{
	auto texture = std::make_shared<Texture>(tex);
	texture->SetType(type);
	m_Material->AddTexture(texture);
	return texture;
}

Material::Material(const aiMaterial* material, Model& model, const std::string& shader)
	: m_Shader(shader)
	, m_Name(material->mName)
{
	LoadTextures(material, model, Texture::Type::Ambient);
	LoadTextures(material, model, Texture::Type::Diffuse);
	LoadTextures(material, model, Texture::Type::Specular);
	LoadTextures(material, model, Texture::Type::Normal);
	LoadTextures(material, model, Texture::Type::Bump);
}


void Material::LoadTextures(const aiMaterial* material, Model& model, Texture::Type type)
{
	aiTextureType aiType = Texture::ConvertType(type);
	uint count = material->GetTextureCount(aiType);
	for (uint i = 0; i < count; ++i) {
		aiString path;
		material->GetTexture(aiType, i, &path);
		std::string spath{path.C_Str()};
		uint pos = spath.find('*');
		ASSERT(pos != std::string::npos);
		const uint id = std::stoi(spath.substr(pos + 1, spath.length() - pos - 1));
		SPtr<Texture> t = model.GetTexture(id);
		t->SetType(type);
		m_Textures.emplace_back(t);
	}
}


aiTextureType Texture::ConvertType(Type type)
{
	switch (type)
	{
	case Texture::Type::Ambient:  return aiTextureType_AMBIENT;  break;
	case Texture::Type::Diffuse:  return aiTextureType_DIFFUSE;  break;
	case Texture::Type::Specular: return aiTextureType_SPECULAR; break;
	case Texture::Type::Normal:   return aiTextureType_NORMALS;  break;
	case Texture::Type::Bump:     return aiTextureType_HEIGHT;	 break;
	
	default: ASSERT_FAIL("Unsupported type"); break;
	}
	return aiTextureType_UNKNOWN;
}


void Texture::Load(void)
{
	if (!IsLoaded()) {
		std::string path = "assets/textures/" + m_Name;
		m_RenderTex = Texture2D::Create(path);
	}
}


void PrimitiveMesh::Init(const glm::mat4& transform, float scale /*= 1.f*/)
{
	SetTransform(transform);

	FillVerticies();
	for (auto& v : m_Verts)
		v.position *= scale;

	FillIndicies();

	SetupRenderable();
}


void Cube::FillVerticies(void)
{
	glm::vec3 v3(0.f);
	glm::vec2 v2(0.f);
	m_Verts = {
		{ { -1.f, -1.f,  1.f }, v3, v3, v3, v2 },
		{ {  1.f, -1.f,  1.f }, v3, v3, v3, v2 },
		{ {  1.f,  1.f,  1.f }, v3, v3, v3, v2 },
		{ { -1.f,  1.f,  1.f }, v3, v3, v3, v2 },
		{ {  1.f, -1.f, -1.f }, v3, v3, v3, v2 },
		{ {  1.f,  1.f, -1.f }, v3, v3, v3, v2 },
		{ { -1.f,  1.f, -1.f }, v3, v3, v3, v2 },
		{ { -1.f, -1.f, -1.f }, v3, v3, v3, v2 },
	};
}


void Cube::FillIndicies(void)
{
	m_Faces = {
		{ 0, 1, 3 },
		{ 1, 2, 3 },
		{ 1, 5, 2 },
		{ 1, 4, 5 },
		{ 6, 5, 4 },
		{ 6, 4, 7 },
		{ 0, 3, 6 },
		{ 0, 6, 7 },
		{ 3, 2, 5 },
		{ 3, 5, 6 },
		{ 0, 4, 1 },
		{ 0, 7, 4 }
	};
}


void Quad::FillVerticies(void)
{
	glm::vec3 v3(0.f);
	m_Verts = {
		{ { -1.f, -1.f,  0.f }, v3, v3, v3, { 0, 0 } },
		{ {  1.f, -1.f,  0.f }, v3, v3, v3, { 1, 0 } },
		{ {  1.f,  1.f,  0.f }, v3, v3, v3, { 1, 1 } },
		{ { -1.f,  1.f,  0.f }, v3, v3, v3, { 0, 1 } },
	};
}

void Quad::FillIndicies(void)
{
	m_Faces = {
		{ 0, 1, 2 },
		{ 2, 3, 0 }
	};
}


}
}