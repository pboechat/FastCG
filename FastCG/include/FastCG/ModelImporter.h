#ifndef FASTCG_MODEL_IMPORTER_H_
#define FASTCG_MODEL_IMPORTER_H_

#include <FastCG/Mesh.h>
#include <FastCG/Material.h>
#include <FastCG/GameObject.h>
#include <FastCG/AABB.h>

#include <glm/glm.hpp>

#include <type_traits>
#include <string>
#include <vector>
#include <cstdint>

namespace FastCG
{
	enum class ModelImporterOption : uint8_t
	{
		MIO_NONE = 0,
		MIO_EXPORT_OPTIMIZED_MODEL_FILE = 1 << 0,

	};

	using ModelImporterOptions = std::underlying_type<ModelImporterOption>::type;

	class ModelImporter
	{
	public:
		static void SetBasePath(const std::string& basePath);
		static GameObject* Import(const std::string& rFileName, ModelImporterOptions options = (ModelImporterOptions)ModelImporterOption::MIO_NONE);
		static void Dispose();

	private:
		ModelImporter() = delete;
		~ModelImporter() = delete;

	};

}

#endif