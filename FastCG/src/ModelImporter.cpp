#include <FastCG/Transform.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MathT.h>
#include <FastCG/FileWriter.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/Exception.h>
#include <FastCG/Colors.h>
#include <FastCG/BinaryStream.h>
#include <FastCG/AssetSystem.h>

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tinyobj_loader_c.h>

#include <vector>
#include <unordered_map>

namespace FastCG
{
	struct OptimizedModelHeader
	{
		size_t numVertices;
		size_t numNormals;
		size_t numUvs;
		size_t numIndices;
	};

	struct ImportContext
	{
		std::vector<std::unique_ptr<char[]>> fileData;
	};

	std::string GetOptimizedModelFilePath(const std::string &rFilePath)
	{
		return File::GetBasePath(rFilePath) + File::GetFileNameWithoutExtension(rFilePath) + ".model";
	}

	void FileReaderCallback(void *context,
							const char *filename,
							int isMtl,
							const char *objFilename,
							char **buffer,
							size_t *length)
	{
		auto *importContext = (ImportContext *)context;
		auto data = FileReader::ReadText(AssetSystem::GetInstance()->Resolve(filename), *length);
		*buffer = data.get();
		importContext->fileData.emplace_back(std::move(data));
	}

	using MeshCatalog = std::unordered_map<size_t, Mesh *>;

	void BuildMeshCatalog(const std::string &rName,
						  const tinyobj_attrib_t &attributes,
						  const tinyobj_shape_t *pShapes,
						  size_t numShapes,
						  MeshCatalog &rMeshCatalog)
	{
		for (size_t shapeIdx = 0; shapeIdx < numShapes; shapeIdx++)
		{
			auto &shape = pShapes[shapeIdx];

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
					auto &face = attributes.faces[faceIdx++];
					if (face.v_idx != (int)0x80000000)
					{
						auto *pVertices = attributes.vertices + (intptr_t)(face.v_idx * 3);
						vertices.emplace_back(glm::vec3{pVertices[0], pVertices[1], pVertices[2]});
					}
					else
					{
						vertices.emplace_back(glm::vec3{0, 0, 0});
					}
					if (face.vn_idx != (int)0x80000000)
					{
						auto *pNormals = attributes.normals + (intptr_t)(face.vn_idx * 3);
						normals.emplace_back(glm::vec3{pNormals[0], pNormals[1], pNormals[2]});
					}
					else
					{
						normals.emplace_back(glm::vec3{0, 0, 0});
						regenNormals = true;
					}
					if (face.vt_idx != (int)0x80000000)
					{
						auto *pUvs = attributes.texcoords + (intptr_t)(face.vt_idx * 2);
						uvs.emplace_back(glm::vec2{pUvs[0], pUvs[1]});
					}
					else
					{
						uvs.emplace_back(glm::vec2{0, 0});
					}
					indices.emplace_back(idx++);
				}
			}

			auto *pMesh = RenderingSystem::GetInstance()->CreateMesh({rName + " (" + std::to_string(shapeIdx) + ")",
																	  vertices,
																	  normals,
																	  uvs,
																	  {},
																	  indices,
																	  true,
																	  regenNormals,
																	  true});
			rMeshCatalog.emplace(shapeIdx, pMesh);
		}
	}

	using MaterialCatalog = std::unordered_map<uint32_t, Material *>;

	void BuildMaterialCatalog(const std::string &rName,
							  const tinyobj_material_t *pMaterials,
							  size_t numMaterials,
							  MaterialCatalog &rMaterialCatalog)
	{
		for (size_t materialIdx = 0; materialIdx < numMaterials; materialIdx++)
		{
			auto &material = pMaterials[materialIdx];

			auto diffuseColor = glm::vec4{material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0};
			auto specularColor = glm::vec4{material.specular[0], material.specular[1], material.specular[2], 1.0};
			auto emissiveColor = glm::vec4{material.emission[0], material.emission[1], material.emission[2], 1.0};
			auto shininess = material.shininess;

			Texture *pColorMapTexture = nullptr;
			if (material.diffuse_texname != nullptr)
			{
				pColorMapTexture = TextureImporter::Import(material.diffuse_texname);
			}

			Texture *pBumpMapTexture = nullptr;
			if (material.bump_texname != nullptr)
			{
				pBumpMapTexture = TextureImporter::Import(material.bump_texname);
			}

			const Shader *pShader;
			if (pBumpMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pShader = RenderingSystem::GetInstance()->FindShader("BumpedSpecular");
				}
				else
				{
					pShader = RenderingSystem::GetInstance()->FindShader("BumpedDiffuse");
				}
			}
			else if (pColorMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pShader = RenderingSystem::GetInstance()->FindShader("Specular");
				}
				else
				{
					pShader = RenderingSystem::GetInstance()->FindShader("Diffuse");
				}
			}
			else
			{
				pShader = RenderingSystem::GetInstance()->FindShader("SolidColor");
			}

			auto *pManagedMaterial = RenderingSystem::GetInstance()->CreateMaterial({rName + " (" + std::to_string(materialIdx) + ")", pShader});
			pManagedMaterial->SetDiffuseColor(diffuseColor);
			pManagedMaterial->SetSpecularColor(specularColor);
			pManagedMaterial->SetShininess(shininess);

			if (pColorMapTexture != nullptr)
			{
				pManagedMaterial->SetColorMap(pColorMapTexture);
			}
			if (pBumpMapTexture != nullptr)
			{
				pManagedMaterial->SetBumpMap(pBumpMapTexture);
			}

			rMaterialCatalog.emplace((uint32_t)materialIdx, pManagedMaterial);
		}
	}

	GameObject *BuildGameObjectFromObj(const std::string &rModelName,
									   const tinyobj_attrib_t &attributes,
									   const tinyobj_shape_t *pShapes,
									   size_t numShapes,
									   const MaterialCatalog &rMaterialCatalog,
									   const MeshCatalog &rMeshCatalog)
	{
		auto *pModelGameObject = GameObject::Instantiate(rModelName);
		auto *pModelTransform = pModelGameObject->GetTransform();
		for (size_t shapeIdx = 0; shapeIdx < numShapes; shapeIdx++)
		{
			auto *pShapeGameObject = GameObject::Instantiate(rModelName + " (" + std::to_string(shapeIdx) + ")");
			pShapeGameObject->GetTransform()->SetParent(pModelTransform);

			Mesh *pMesh = nullptr;
			{
				auto it = rMeshCatalog.find(shapeIdx);
				assert(it != rMeshCatalog.end());
				pMesh = it->second;
			}

			// doesn't support multi materials at the moment
			auto &shape = pShapes[shapeIdx];
			auto materialIdx = attributes.material_ids[shapeIdx];
			Material *pMaterial = nullptr;
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

			auto *pRenderable = Renderable::Instantiate(pShapeGameObject, pMaterial, pMesh);
		}
		return pModelGameObject;
	}

	void CombineMeshes(const GameObject *pGameObject,
					   std::vector<glm::vec3> &rVertices,
					   std::vector<glm::vec3> &rNormals,
					   std::vector<glm::vec2> &rUVs,
					   std::vector<uint32_t> &rIndices)
	{
		auto *pRenderable = static_cast<Renderable *>(pGameObject->GetComponent(Renderable::TYPE));
		if (pRenderable != nullptr)
		{
			auto model = pGameObject->GetTransform()->GetModel();

			const auto *pMesh = pRenderable->GetMesh();
			auto indicesOffset = (uint32_t)rVertices.size();
			const auto &rMeshVertices = pMesh->GetVertices();
			for (auto &vertex : rMeshVertices)
			{
				rVertices.emplace_back(glm::vec3(model * glm::vec4(vertex, 1)));
			}

			const auto &rMeshNormals = pMesh->GetNormals();
			rNormals.insert(rNormals.end(), rMeshNormals.begin(), rMeshNormals.end());

			const auto &rMeshUVs = pMesh->GetUVs();
			rUVs.insert(rUVs.end(), rMeshUVs.begin(), rMeshUVs.end());

			const auto &rMeshIndices = pMesh->GetIndices();
			for (auto &index : rMeshIndices)
			{
				rIndices.emplace_back(indicesOffset + index);
			}
		}

		const auto &rChildren = pGameObject->GetTransform()->GetChildren();
		for (auto *child : rChildren)
		{
			CombineMeshes(child->GetGameObject(), rVertices, rNormals, rUVs, rIndices);
		}
	}

	void ExportOptimizedModelFile(const std::string &rOptimizedModelFileName, const GameObject *pGameObject)
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

	GameObject *ImportModelFromObjFile(const std::string &rFilePath)
	{
		tinyobj_attrib_t attributes;
		tinyobj_shape_t *pShapes;
		tinyobj_material_t *pMaterials;
		size_t numShapes, numMaterials;

		ImportContext importContext{};

		if (tinyobj_parse_obj(&attributes,
							  &pShapes,
							  &numShapes,
							  &pMaterials,
							  &numMaterials,
							  rFilePath.c_str(),
							  &FileReaderCallback,
							  (void *)&importContext,
							  TINYOBJ_FLAG_TRIANGULATE) != TINYOBJ_SUCCESS)
		{
			return nullptr;
		}

		MaterialCatalog materialCatalog;
		BuildMaterialCatalog(rFilePath, pMaterials, numMaterials, materialCatalog);

		MeshCatalog meshCatalog;
		BuildMeshCatalog(rFilePath, attributes, pShapes, numShapes, meshCatalog);

		auto modelName = File::GetFileNameWithoutExtension(rFilePath);
		auto *pModelGameObject = BuildGameObjectFromObj(modelName, attributes, pShapes, numShapes, materialCatalog, meshCatalog);

		tinyobj_attrib_free(&attributes);
		tinyobj_shapes_free(pShapes, numShapes);
		tinyobj_materials_free(pMaterials, numMaterials);

		return pModelGameObject;
	}

	GameObject *ImportModelFromOptimizedFile(const std::string &rOptimizedModelFilePath)
	{
		size_t dataSize;
		auto data = FileReader::ReadBinary(rOptimizedModelFilePath, dataSize);

		if (data == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error opening optimized model file: %s", rOptimizedModelFilePath.c_str());
		}

		InputBinaryStream inStream(data.get(), dataSize);

		OptimizedModelHeader header;

		inStream.Read(header);

		std::vector<glm::vec3> vertices;
		vertices.resize(header.numVertices);
		if (!inStream.Read(&vertices[0], vertices.size()))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error reading vertices from optimized model file: %s", rOptimizedModelFilePath.c_str());
		}

		std::vector<glm::vec3> normals;
		normals.resize(header.numNormals);
		if (!inStream.Read(&normals[0], normals.size()))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error reading normals from optimized model file: %s", rOptimizedModelFilePath.c_str());
		}

		std::vector<glm::vec2> uvs;
		uvs.resize(header.numUvs);
		if (!inStream.Read(&uvs[0], uvs.size()))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error reading uvs from optimized model file: %s", rOptimizedModelFilePath.c_str());
		}

		std::vector<uint32_t> indices;
		indices.resize(header.numIndices);
		if (!inStream.Read(&indices[0], indices.size()))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error reading indices from optimized model file: %s", rOptimizedModelFilePath.c_str());
		}

		auto *pMesh = RenderingSystem::GetInstance()->CreateMesh({rOptimizedModelFilePath,
																  vertices,
																  normals,
																  uvs,
																  {},
																  indices,
																  true,
																  false,
																  true});
		const auto *pShader = RenderingSystem::GetInstance()->FindShader("SolidColor");

		auto modelName = File::GetFileNameWithoutExtension(rOptimizedModelFilePath);
		auto *pMaterial = RenderingSystem::GetInstance()->CreateMaterial({modelName + " Optimized Material", pShader});
		pMaterial->SetDiffuseColor(Colors::WHITE);
		pMaterial->SetSpecularColor(Colors::BLACK);
		pMaterial->SetShininess(0);

		auto *pGameObject = GameObject::Instantiate(modelName);
		Renderable::Instantiate(pGameObject, pMaterial, pMesh);

		return pGameObject;
	}

	GameObject *ModelImporter::Import(const std::string &rFilePath, ModelImporterOptionType options)
	{
		auto optimizedFilePath = AssetSystem::GetInstance()->Resolve(GetOptimizedModelFilePath(rFilePath));
		if (File::Exists(optimizedFilePath))
		{
			return ImportModelFromOptimizedFile(optimizedFilePath);
		}
		else
		{
			auto *pModelGameObject = ImportModelFromObjFile(rFilePath);
			if ((options & (uint8_t)ModelImporterOption::MIO_EXPORT_OPTIMIZED_MODEL_FILE) != 0)
			{
				ExportOptimizedModelFile(optimizedFilePath, pModelGameObject);
			}
			return pModelGameObject;
		}
	}

}
