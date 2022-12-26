#include <FastCG/Transform.h>
#include <FastCG/TextureLoader.h>
#include <FastCG/ShaderConstants.h>
#include <FastCG/Renderable.h>
#include <FastCG/ModelLoader.h>
#include <FastCG/MeshUtils.h>
#include <FastCG/MathT.h>
#include <FastCG/GraphicsSystem.h>
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
	struct LoadContext
	{
		std::string basePath;
		std::vector<std::unique_ptr<char[]>> fileData;
	};

	void FileReaderCallback(void *pContext,
							const char *pFilename,
							int isMtl,
							const char *pObjFilename,
							char **pBuffer,
							size_t *pLength)
	{
		auto *pLoadContext = (LoadContext *)pContext;
		auto data = FileReader::ReadText(File::Join({pLoadContext->basePath, pFilename}), *pLength);
		*pBuffer = data.get();
		pLoadContext->fileData.emplace_back(std::move(data));
	}

	using MeshCatalog = std::unordered_map<size_t, Mesh *>;

	void BuildMeshCatalog(const std::string &rFilePath,
						  const tinyobj_attrib_t &attributes,
						  const tinyobj_shape_t *pShapes,
						  size_t numShapes,
						  MeshCatalog &rMeshCatalog)
	{
		auto name = File::GetFileNameWithoutExtension(rFilePath);
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

			auto *pMesh = GraphicsSystem::GetInstance()->CreateMesh({name + " (" + std::to_string(shapeIdx) + ")",
																	 {{"Positions", BufferUsage::STATIC, positions.size() * sizeof(glm::vec3), positions.data(), {{POSITION_SHADER_INPUT_INDEX, 3, VertexDataType::FLOAT, false, 0, 0}}},
																	  {"Normals", BufferUsage::STATIC, normals.size() * sizeof(glm::vec3), normals.data(), {{NORMAL_SHADER_INPUT_INDEX, 3, VertexDataType::FLOAT, false, 0, 0}}},
																	  {"UVs", BufferUsage::STATIC, uvs.size() * sizeof(glm::vec2), uvs.data(), {{UV_SHADER_INPUT_INDEX, 2, VertexDataType::FLOAT, true, 0, 0}}},
																	  {"Tangents", BufferUsage::STATIC, tangents.size() * sizeof(glm::vec4), tangents.data(), {{TANGENT_SHADER_INPUT_INDEX, 4, VertexDataType::FLOAT, false, 0, 0}}}},
																	 {BufferUsage::STATIC, (uint32_t)indices.size(), indices.data()},
																	 MeshUtils::CalculateBounds(positions)});
			rMeshCatalog.emplace(shapeIdx, pMesh);
		}
	}

	using MaterialCatalog = std::unordered_map<uint32_t, Material *>;

	void BuildMaterialCatalog(const std::string &rFilePath,
							  const tinyobj_material_t *pMaterials,
							  size_t numMaterials,
							  MaterialCatalog &rMaterialCatalog)
	{
		auto name = File::GetFileNameWithoutExtension(rFilePath);
		auto basePath = File::GetBasePath(rFilePath);
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
				pColorMapTexture = TextureLoader::Load(File::Join({basePath, material.diffuse_texname}));
			}

			Texture *pBumpMapTexture = nullptr;
			if (material.bump_texname != nullptr)
			{
				pBumpMapTexture = TextureLoader::Load(File::Join({basePath, material.bump_texname}));
			}

			const MaterialDefinition *pMaterialDefinition;
			if (pBumpMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pMaterialDefinition = GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueBumpedSpecular");
				}
				else
				{
					pMaterialDefinition = GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueBumpedDiffuse");
				}
			}
			else if (pColorMapTexture != nullptr)
			{
				// TODO:
				if (shininess != 0.0f)
				{
					pMaterialDefinition = GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueSpecular");
				}
				else
				{
					pMaterialDefinition = GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueDiffuse");
				}
			}
			else
			{
				pMaterialDefinition = GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueSolidColor");
			}

			auto *pManagedMaterial = GraphicsSystem::GetInstance()->CreateMaterial({name + " (" + std::to_string(materialIdx) + ")", pMaterialDefinition});
			pManagedMaterial->SetConstant("uDiffuseColor", diffuseColor);
			pManagedMaterial->SetConstant("uSpecularColor", specularColor);
			pManagedMaterial->SetConstant("uShininess", shininess);

			if (pColorMapTexture != nullptr)
			{
				pManagedMaterial->SetTexture("uColorMap", pColorMapTexture);
			}
			if (pBumpMapTexture != nullptr)
			{
				pManagedMaterial->SetTexture("uBumpMap", pBumpMapTexture);
			}

			rMaterialCatalog.emplace((uint32_t)materialIdx, pManagedMaterial);
		}
	}

	GameObject *BuildGameObjectFromObj(const std::string &rModelName,
									   const tinyobj_attrib_t &attributes,
									   const tinyobj_shape_t *pShapes,
									   size_t numShapes,
									   const MaterialCatalog &rMaterialCatalog,
									   const MeshCatalog &rMeshCatalog,
									   const Material *pDefaultMaterial,
									   ModelLoaderOptionMaskType options)
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
			const Material *pMaterial = nullptr;
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
			else
			{
				pMaterial = pDefaultMaterial;
			}

			Renderable::Instantiate(pShapeGameObject,
									pMaterial,
									pMesh,
									(options & (ModelLoaderOptionMaskType)ModelLoaderOption::IS_SHADOW_CASTER) != 0);
		}
		return pModelGameObject;
	}

	GameObject *ModelLoader::Load(const std::string &rFilePath, const Material *pDefaultMaterial, ModelLoaderOptionMaskType options)
	{
		tinyobj_attrib_t attributes;
		tinyobj_shape_t *pShapes;
		tinyobj_material_t *pMaterials;
		size_t numShapes, numMaterials;

		LoadContext loadContext{File::GetBasePath(rFilePath)};

		if (tinyobj_parse_obj(&attributes,
							  &pShapes,
							  &numShapes,
							  &pMaterials,
							  &numMaterials,
							  File::GetFileName(rFilePath).c_str(),
							  &FileReaderCallback,
							  (void *)&loadContext,
							  TINYOBJ_FLAG_TRIANGULATE) != TINYOBJ_SUCCESS)
		{
			return nullptr;
		}

		MaterialCatalog materialCatalog;
		BuildMaterialCatalog(rFilePath, pMaterials, numMaterials, materialCatalog);

		MeshCatalog meshCatalog;
		BuildMeshCatalog(rFilePath, attributes, pShapes, numShapes, meshCatalog);

		auto modelName = File::GetFileNameWithoutExtension(rFilePath);
		auto *pModelGameObject = BuildGameObjectFromObj(modelName, attributes, pShapes, numShapes, materialCatalog, meshCatalog, pDefaultMaterial, options);

		tinyobj_attrib_free(&attributes);
		tinyobj_shapes_free(pShapes, numShapes);
		tinyobj_materials_free(pMaterials, numMaterials);

		return pModelGameObject;
	}

}
