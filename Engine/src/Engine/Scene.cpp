#include "pch.h"
#include "Scene.h"
#include "Renderer/Renderer.h"

Engine::SPtr<Engine::Texture2D> GlobalTexture;

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
		unsigned char* data = reinterpret_cast<unsigned char*>(aiTex->pcData);
		//auto& t = m_Textures.emplace_back(std::make_shared<Texture>(aiTex->mWidth, aiTex->mHeight, data));
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
	vbo->SetLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float3, "a_Normal" },
		{ Engine::ShaderDataType::Float3, "a_Tangent" },
		{ Engine::ShaderDataType::Float3, "a_Bitangent" },
		{ Engine::ShaderDataType::Float2, "a_UV" }
	});
	m_VAO->AddVertexBuffer(vbo);

	std::vector<uint> inds;
	GetIndecies(inds);
	SPtr<IndexBuffer> ebo;
	ebo.reset(Engine::IndexBuffer::Create(inds.data(), inds.size()));
	m_VAO->SetIndexBuffer(ebo);
}


void Mesh::Render(void)
{
	
}


void Mesh::Render(const SPtr<Shader>& shader)
{
	shader->Bind();
	shader->UploadUniformFloat("u_material.shininess", 32.f);

	for (const auto& t : m_Material->GetTextures()) {
		switch (t->GetType())
		{
		case Texture::Type::Diffuse:
			shader->UploadUniformInt("u_material.diffuse", 0);
			t->GetRenderTex()->Bind(0);
			//GlobalTexture->Bind(0);
			break;
		case Texture::Type::Specular:
			shader->UploadUniformInt("u_material.specular", 1);
			t->GetRenderTex()->Bind(1);
			//GlobalTexture->Bind(1);
			break;
		}
	}

	Renderer::Submit(shader, m_VAO, m_WorldTransform);
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


Material::Material(const aiMaterial* material, Model& model, const std::string& shader)
	: m_Shader(shader)
{
	LoadTextures(material, model, Texture::Type::Ambient);
	LoadTextures(material, model, Texture::Type::Diffuse);
	LoadTextures(material, model, Texture::Type::Specular);
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
	
	default: ASSERT_FAIL("Unsupported type"); break;
	}
	return aiTextureType_UNKNOWN;
}


void Texture::Load(void)
{
	if (!IsLoaded()) {
		std::string path = "assets/textures2/" + m_Name;
		m_RenderTex = Texture2D::Create(path);
	}
}


}
}