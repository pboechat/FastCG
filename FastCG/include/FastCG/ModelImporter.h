#ifndef FASTCG_MODEL_IMPORTER_H
#define FASTCG_MODEL_IMPORTER_H

#include <FastCG/GameObject.h>

#include <type_traits>
#include <string>
#include <cstdint>

namespace FastCG
{
	enum class ModelImporterOption : uint8_t
	{
		NONE = 0,
		IS_SHADOW_CASTER = 1 << 0

	};

	using ModelImporterOptionIntType = std::underlying_type<ModelImporterOption>::type;
	using ModelImporterOptionMaskType = uint8_t;

	class ModelImporter
	{
	public:
		static GameObject *Import(const std::string &rFileName, const Material *pDefaultMaterial, ModelImporterOptionMaskType options = (ModelImporterOptionMaskType)ModelImporterOption::NONE);

	private:
		ModelImporter() = delete;
		~ModelImporter() = delete;
	};

}

#endif