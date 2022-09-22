#include <FastCG/Transform.h>
#include <FastCG/Texture.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/MeshFilter.h>
#include <FastCG/MathT.h>
#include <FastCG/FileWriter.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/Exception.h>
#include <FastCG/Colors.h>
#include <FastCG/BinaryStream.h>

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tinyobj_loader_c.h>

#include <vector>
#include <map>

namespace FastCG
{
	static std::string gBasePath;
	static std::vector<std::shared_ptr<Material>> gManagedMaterials;
	static std::vector<std::shared_ptr<Mesh>> gManagedMeshes;

	struct OptimizedModelHeader
	{
		size_t numVertices;
		size_t numNormals;
		size_t numUvs;
		size_t numIndices;

	};

	struct ImportContext
	{
		std::string basePath;
		std::vector<std::unique_ptr<char[]>> fileData;

	};

	std::string GetOptimizedModelFileName(const std::string& rFileName)
	{
		return File::GetFileNameWithoutExtension(rFileName) + ".model";
	}

	void FileReaderCallback(void* context,
		const char* filename,
		int isMtl,
		const char* objFilename,
		char** buffer,
		size_t* length)
	{
		auto* importContext = (ImportContext*)context;
		auto data = FileReader::ReadText(importContext->basePath + "/" + filename, *length);
		*buffer = data.get();
		importContext->fileData.emplace_back(std::move(data));
	}

	using MeshCatalog = std::map<size_t, std::shared_ptr<Mesh>>;

	void BuildMeshCatalog(const tinyobj_attrib_t& attributes,
		const tinyobj_shape_t* pShapes,
		size_t numShapes,
		MeshCatalog& rMeshCatalog)
	{
		for (size_t shapeIdx = 0; shapeIdx < numShapes; shapeIdx++)
		{
			auto& shape = pShapes[shapeIdx];

			std::vector<glm::vec3> vertices;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> uvs;
			std::vector<uint32_t> indices;

			uint32_t idx = 0;
			uint32_t shapeFaceIdx = 0;
			uint32_t faceIdx = shape.face_offset;
			bool regenNormals = false;
			while (shapeFaceIdx < shape.length)
			{
				auto numVertices = attributes.face_num_verts[shapeFaceIdx++];
				// only support triangle meshes at the moment
				assert(numVertices == 3);
				int vertIdx = 0;
				while (vertIdx++ < numVertices)
				{
					auto& face = attributes.faces[faceIdx++];
					if (face.v_idx != (int)0x80000000)
					{
						auto* pVertices = attributes.vertices + (intptr_t)(face.v_idx * 3);
						vertices.emplace_back(glm::vec3{ pVertices[0], pVertices[1], pVertices[2] });
					}
					else
					{
						vertices.emplace_back(glm::vec3{ 0, 0, 0 });
					}
					if (face.vn_idx != (int)0x80000000)
					{
						auto* pNormals = attributes.normals + (intptr_t)(face.vn_idx * 3);
						normals.emplace_back(glm::vec3{ pNormals[0], pNormals[1], pNormals[2] });
					}
					else
					{
						normals.emplace_back(glm::vec3{ 0, 0, 0 });
						regenNormals = true;
					}
					if (face.vt_idx != (int)0x80000000)
					{
						auto* pUvs = attributes.texcoords + (intptr_t)(face.vt_idx * 2);
						uvs.emplace_back(glm::vec2{ pUvs[0], pUvs[1] });
					}
					else
					{
						uvs.emplace_back(glm::vec2{ 0, 0 });
					}
					indices.emplace_back(idx++);
				}
			}

			auto pMesh = std::make_shared<Mesh>(vertices, normals, uvs, indices);
			if (regenNormals)
			{
				pMesh->CalculateNormals();
			}
			pMesh->CalculateTangents();

			rMeshCatalog.emplace(shapeIdx, pMesh);
			gManagedMeshes.emplace_back(pMesh);
		}
	}

	using MaterialCatalog = std::map<uint32_t, std::shared_ptr<Material>>;

	void BuildMaterialCatalog(const std::string& rBasePath,
		const tinyobj_material_t* pMaterials,
		size_t numMaterials,
		MaterialCatalog& rMaterialCatalog)
	{
		for (size_t materialIdx = 0; materialIdx < numMaterials; materialIdx++)
		{
			auto& material = pMaterials[materialIdx];

			auto diffuseColor = glm::vec4{ material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0 };
			auto specularColor = glm::vec4{ material.specular[0], material.specular[1], material.specular[2], 1.0 };
			auto emissiveColor = glm::vec4{ material.emission[0], material.emission[1], material.emission[2], 1.0 };
			auto shininess = material.shininess;

			std::shared_ptr<Texture> pColorMapTexture = nullptr;
			if (material.diffuse_texname != nullptr)
			{
				pColorMapTexture = TextureImporter::Import(material.diffuse_texname);
			}

			std::shared_ptr<Texture> pBumpMapTexture = nullptr;
			if (material.bump_texname != nullptr)
			{
				pBumpMapTexture = TextureImporter::Import(material.bump_texname);
			}

			std::shared_ptr<Shader> pShader;
			if (pBumpMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pShader = ShaderRegistry::Find("BumpedSpecular");
				}
				else
				{
					pShader = ShaderRegistry::Find("BumpedDiffuse");
				}
			}
			else if (pColorMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pShader = ShaderRegistry::Find("Specular");
				}
				else
				{
					pShader = ShaderRegistry::Find("Diffuse");
				}
			}
			else
			{
				pShader = ShaderRegistry::Find("SolidColor");
			}

			auto pManagedMaterial = std::make_shared<Material>(pShader);
			pManagedMaterial->SetVec4("diffuseColor", diffuseColor);
			pManagedMaterial->SetVec4("specularColor", specularColor);
			pManagedMaterial->SetVec4("emissiveColor", emissiveColor);
			pManagedMaterial->SetFloat("shininess", shininess);

			if (pColorMapTexture != nullptr)
			{
				pManagedMaterial->SetTexture("colorMap", pColorMapTexture);
			}
			if (pBumpMapTexture != nullptr)
			{
				pManagedMaterial->SetTexture("bumpMap", pBumpMapTexture);
			}

			rMaterialCatalog.emplace((uint32_t)materialIdx, pManagedMaterial);
			gManagedMaterials.emplace_back(pManagedMaterial);
		}
	}

	GameObject* BuildGameObjectFromObj(const std::string& rModelName,
		const tinyobj_attrib_t& attributes,
		const tinyobj_shape_t* pShapes,
		size_t numShapes,
		const MaterialCatalog& rMaterialCatalog,
		const MeshCatalog& rMeshCatalog)
	{
		auto* pModelGameObject = GameObject::Instantiate(rModelName);
		auto* pModelTransform = pModelGameObject->GetTransform();
		for (size_t shapeIdx = 0; shapeIdx < numShapes; shapeIdx++)
		{
			auto* pShapeGameObject = GameObject::Instantiate(rModelName + " (" + std::to_string(shapeIdx) + ")");
			pShapeGameObject->GetTransform()->SetParent(pModelTransform);
			auto* pMeshRenderer = MeshRenderer::Instantiate(pShapeGameObject);
			auto* pMeshFilter = MeshFilter::Instantiate(pShapeGameObject);

			{
				auto it = rMeshCatalog.find(shapeIdx);
				assert(it != rMeshCatalog.end());
				pMeshRenderer->AddMesh(it->second);
			}

			// doesn't support multi materials at the moment
			auto& shape = pShapes[shapeIdx];
			auto materialIdx = attributes.material_ids[shapeIdx];
			std::shared_ptr<Material> pMaterial = nullptr;
			if (materialIdx > 0)
			{
				auto it = rMaterialCatalog.find((uint32_t)materialIdx);
				assert(it != rMaterialCatalog.end());
				pMaterial = it->second;
			}
			else if (!rMaterialCatalog.empty())
			{
				pMaterial = rMaterialCatalog.begin()->second;
			}
			if (pMaterial != nullptr)
			{
				pMeshFilter->SetMaterial(pMaterial);
			}
		}
		return pModelGameObject;
	}

	void CombineMeshes(const GameObject* pGameObject,
		std::vector<glm::vec3>& rVertices,
		std::vector<glm::vec3>& rNormals,
		std::vector<glm::vec2>& rUVs,
		std::vector<uint32_t>& rIndices
	)
	{
		auto* pMeshRenderer = static_cast<MeshRenderer*>(pGameObject->GetComponent(MeshRenderer::TYPE));
		if (pMeshRenderer != nullptr)
		{
			auto& rModel = pGameObject->GetTransform()->GetModel();

			auto& rMeshes = pMeshRenderer->GetMeshes();
			for (const auto& pMesh : rMeshes)
			{
				auto indicesOffset = (uint32_t)rVertices.size();
				const auto& rMeshVertices = pMesh->GetVertices();
				for (auto& vertex : rMeshVertices)
				{
					rVertices.emplace_back(glm::vec3(rModel * glm::vec4(vertex, 1)));
				}

				const auto& rMeshNormals = pMesh->GetNormals();
				rNormals.insert(rNormals.end(), rMeshNormals.begin(), rMeshNormals.end());

				const auto& rMeshUVs = pMesh->GetUVs();
				rUVs.insert(rUVs.end(), rMeshUVs.begin(), rMeshUVs.end());

				const auto& rMeshIndices = pMesh->GetIndices();
				for (auto& index : rMeshIndices)
				{
					rIndices.emplace_back(indicesOffset + index);
				}
			}
		}

		const auto& rChildren = pGameObject->GetTransform()->GetChildren();
		for (auto* child : rChildren)
		{
			CombineMeshes(child->GetGameObject(), rVertices, rNormals, rUVs, rIndices);
		}
	}

	void ExportOptimizedModelFile(const std::string& rOptimizedModelFileName, const GameObject* pGameObject)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<uint32_t> indices;

		CombineMeshes(pGameObject, vertices, normals, uvs, indices);

		OptimizedModelHeader header;
		header.numVertices = vertices.size();
		header.numNormals = normals.size();
		header.numUvs = uvs.size();
		header.numIndices = indices.size();

		OutputBinaryStream outStream;

		outStream.Write(header);

		outStream.Write(&vertices[0], vertices.size());
		outStream.Write(&normals[0], normals.size());
		outStream.Write(&uvs[0], uvs.size());
		outStream.Write(&indices[0], indices.size());

		FileWriter::WriteBinary(rOptimizedModelFileName, outStream.GetData(), outStream.GetSize());
	}

	GameObject* ImportModelFromObjFile(const std::string& rBasePath, const std::string& rFileName, ModelImporterOptions options)
	{
		tinyobj_attrib_t attributes;
		tinyobj_shape_t* pShapes;
		tinyobj_material_t* pMaterials;
		size_t numShapes, numMaterials;

		ImportContext importContext{ rBasePath };

		if (tinyobj_parse_obj(&attributes,
			&pShapes,
			&numShapes,
			&pMaterials,
			&numMaterials,
			rFileName.c_str(),
			&FileReaderCallback,
			(void*)&importContext,
			TINYOBJ_FLAG_TRIANGULATE) != TINYOBJ_SUCCESS)
		{
			return nullptr;
		}

		MaterialCatalog materialCatalog;
		BuildMaterialCatalog(rBasePath, pMaterials, numMaterials, materialCatalog);

		MeshCatalog meshCatalog;
		BuildMeshCatalog(attributes, pShapes, numShapes, meshCatalog);

		auto modelName = File::GetFileNameWithoutExtension(rFileName);
		auto* pModelGameObject = BuildGameObjectFromObj(modelName, attributes, pShapes, numShapes, materialCatalog, meshCatalog);

		if ((options & (uint8_t)ModelImporterOption::MIO_EXPORT_OPTIMIZED_MODEL_FILE) != 0)
		{
			ExportOptimizedModelFile(rFileName, pModelGameObject);
		}

		tinyobj_attrib_free(&attributes);
		tinyobj_shapes_free(pShapes, numShapes);
		tinyobj_materials_free(pMaterials, numMaterials);

		return pModelGameObject;
	}

	GameObject* ImportModelFromOptimizedFile(const std::string& rBasePath, const std::string& rOptimizedModelFileName)
	{
		size_t dataSize;
		auto data = FileReader::ReadBinary(rBasePath + "/" + rOptimizedModelFileName, dataSize);

		if (data == nullptr)
		{
			THROW_EXCEPTION(Exception, "Error opening optimized model file: %s", rOptimizedModelFileName.c_str());
		}

		InputBinaryStream inStream(data.get(), dataSize);

		OptimizedModelHeader header;

		inStream.Read(header);

		std::vector<glm::vec3> vertices;
		vertices.resize(header.numVertices);
		if (!inStream.Read(&vertices[0], vertices.size()))
		{
			THROW_EXCEPTION(Exception, "Error reading vertices from optimized model file: %s", rOptimizedModelFileName.c_str());
		}

		std::vector<glm::vec3> normals;
		normals.resize(header.numNormals);
		if (!inStream.Read(&normals[0], normals.size()))
		{
			THROW_EXCEPTION(Exception, "Error reading normals from optimized model file: %s", rOptimizedModelFileName.c_str());
		}

		std::vector<glm::vec2> uvs;
		uvs.resize(header.numUvs);
		if (!inStream.Read(&uvs[0], uvs.size()))
		{
			THROW_EXCEPTION(Exception, "Error reading uvs from optimized model file: %s", rOptimizedModelFileName.c_str());
		}

		std::vector<uint32_t> indices;
		indices.resize(header.numIndices);
		if (!inStream.Read(&indices[0], indices.size()))
		{
			THROW_EXCEPTION(Exception, "Error reading indices from optimized model file: %s", rOptimizedModelFileName.c_str());
		}

		auto pMesh = std::make_shared<Mesh>(vertices, normals, uvs, indices);
		gManagedMeshes.emplace_back(pMesh);

		auto pMaterial = std::make_shared<Material>(ShaderRegistry::Find("SolidColor"));
		pMaterial->SetVec4("diffuseColor", Colors::WHITE);
		pMaterial->SetVec4("specularColor", Colors::BLACK);
		pMaterial->SetFloat("shininess", 0);

		gManagedMaterials.emplace_back(pMaterial);

		auto* pGameObject = GameObject::Instantiate(File::GetFileNameWithoutExtension(rOptimizedModelFileName));
		auto* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
		auto* pMeshFilter = MeshFilter::Instantiate(pGameObject);

		pMeshFilter->SetMaterial(pMaterial);
		pMeshRenderer->AddMesh(pMesh);

		return pGameObject;
	}

	void ModelImporter::SetBasePath(const std::string& basePath)
	{
		gBasePath = basePath;
	}

	GameObject* ModelImporter::Import(const std::string& rFileName, ModelImporterOptions options)
	{
		auto rOptimizedFileName = GetOptimizedModelFileName(rFileName);
		if (File::Exists(rOptimizedFileName))
		{
			return ImportModelFromOptimizedFile(gBasePath, rOptimizedFileName);
		}
		else
		{
			return ImportModelFromObjFile(gBasePath, rFileName, options);
		}
	}

	void ModelImporter::Dispose()
	{
		gManagedMaterials.clear();
		gManagedMeshes.clear();
	}

}
