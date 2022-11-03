#include <FastCG/Transform.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MeshUtils.h>
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
	struct ImportContext
	{
		std::vector<std::unique_ptr<char[]>> fileData;
	};

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

			std::vector<glm::vec3> positions;
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
						auto *pPositions = attributes.vertices + (intptr_t)(face.v_idx * 3);
						positions.emplace_back(glm::vec3{pPositions[0], pPositions[1], pPositions[2]});
					}
					else
					{
						positions.emplace_back(glm::vec3{0, 0, 0});
					}
					if (face.vn_idx != (int)0x80000000)
					{
						auto *pNormals = attributes.normals + (intptr_t)(face.vn_idx * 3);
						normals.emplace_back(glm::vec3{pNormals[0], pNormals[1], pNormals[2]});
					}
					else if (!regenNormals)
					{
						normals.clear();
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

			if (regenNormals)
			{
				normals = MeshUtils::CalculateNormals(positions, indices);
			}
			auto tangents = MeshUtils::CalculateTangents(positions, normals, uvs, indices);

			auto *pMesh = RenderingSystem::GetInstance()->CreateMesh({rName + " (" + std::to_string(shapeIdx) + ")",
																	  {{"Positions", BufferUsage::STATIC, positions.size() * sizeof(glm::vec3), positions.data(), {{Shader::POSITIONS_ATTRIBUTE_INDEX, 3, VertexDataType::FLOAT, false, 0, 0}}},
																	   {"Normals", BufferUsage::STATIC, normals.size() * sizeof(glm::vec3), normals.data(), {{Shader::NORMALS_ATTRIBUTE_INDEX, 3, VertexDataType::FLOAT, false, 0, 0}}},
																	   {"UVs", BufferUsage::STATIC, uvs.size() * sizeof(glm::vec2), uvs.data(), {{Shader::UVS_ATTRIBUTE_INDEX, 2, VertexDataType::FLOAT, true, 0, 0}}},
																	   {"Tangents", BufferUsage::STATIC, tangents.size() * sizeof(glm::vec4), tangents.data(), {{Shader::TANGENTS_ATTRIBUTE_INDEX, 4, VertexDataType::FLOAT, false, 0, 0}}}},
																	  {BufferUsage::STATIC, (uint32_t)indices.size(), indices.data()},
																	  MeshUtils::CalculateBounds(positions)});
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

	GameObject *ModelImporter::Import(const std::string &rFilePath, ModelImporterOptionType options)
	{
		return ImportModelFromObjFile(rFilePath);
	}

}
