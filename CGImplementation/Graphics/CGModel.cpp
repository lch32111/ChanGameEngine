#include <Graphics/CGModel.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/Shader.h>
#include <Graphics/GLTextureUtility.h>

CGProj::CGModel::CGModel()
	: m_isLoaded(false), m_directory("")
{ }

CGProj::CGModel::CGModel(const char * path)
	: m_isLoaded(false), m_directory(path)
{ }

void CGProj::CGModel::destroy()
{
	if (m_isLoaded)
	{
		for (unsigned int i = 0; i < m_meshes.size(); ++i)
			m_meshes[i].destroy();
	}
}


bool CGProj::CGModel::loadModel()
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile
	(
		m_directory, 
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_FlipUVs
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
		return false;
	}

	m_directory = m_directory.substr(0, m_directory.find_last_of('/'));

	processNode(scene->mRootNode, scene);
	m_isLoaded = true;
	return true;
}


void CGProj::CGModel::deferredFirstRender(Shader* shader)
{
	for (unsigned i = 0; i < m_meshes.size(); ++i)
	{
		m_meshes[i].deferredFirstRender(shader);
	}
}

void CGProj::CGModel::processNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene);
}

CGProj::CGModelMesh CGProj::CGModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordintaes

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinate?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
		if (mesh->mTangents)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		else
			vertex.Tangent = glm::vec3(0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "emissive");
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalMap");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// depthMap -> heightMap for the key of textures
		std::vector<Texture> depthMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "heighthMap");
		textures.insert(textures.end(), depthMaps.begin(), depthMaps.end());

		// std::vector<Texture> reflectMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "LMreflect");
		// textures.insert(textures.end(), reflectMaps.begin(), reflectMaps.end());
	}

	return CGModelMesh(vertices, indices, textures);
}

std::vector<CGProj::Texture> CGProj::CGModel::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;

		for (unsigned int j = 0; j < m_texture_loaded.size(); ++j)
		{
			if (std::strcmp(m_texture_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(m_texture_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			std::string Newpath = m_directory + "/" + str.C_Str();

			Texture texture;
			texture.id = TextureFromFile(Newpath, true);
			texture.key = (1 << (int)(typeName[0] - 'a'));
			texture.path = str.C_Str();
			textures.push_back(texture);
			m_texture_loaded.push_back(texture);
		}
	}

	return textures;
}
