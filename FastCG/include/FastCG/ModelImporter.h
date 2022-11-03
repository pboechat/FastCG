#ifndef FASTCG_MODEL_IMPORTER_H
#define FASTCG_MODEL_IMPORTER_H

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
		NONE = 0,

	};

	using ModelImporterOptionType = std::underlying_type<ModelImporterOption>::type;

	class ModelImporter
	{
	public:
		static GameObject *Import(const std::string &rFileName, ModelImporterOptionType options = (ModelImporterOptionType)ModelImporterOption::NONE);

	private:
		ModelImporter() = delete;
		~ModelImporter() = delete;
	};

}

#endif