#ifndef FASTCG_MODEL_LOADER_H
#define FASTCG_MODEL_LOADER_H

#include <FastCG/GameObject.h>

#include <type_traits>
#include <string>
#include <cstdint>

namespace FastCG
{
	enum class ModelLoaderOption : uint8_t
	{
		NONE = 0,
		IS_SHADOW_CASTER = 1 << 0

	};

	using ModelLoaderOptionIntType = std::underlying_type<ModelLoaderOption>::type;
	using ModelLoaderOptionMaskType = uint8_t;

	class ModelLoader
	{
	public:
		static GameObject *Load(const std::string &rFileName, const Material *pDefaultMaterial, ModelLoaderOptionMaskType options = (ModelLoaderOptionMaskType)ModelLoaderOption::NONE);

	private:
		ModelLoader() = delete;
		~ModelLoader() = delete;
	};

}

#endif