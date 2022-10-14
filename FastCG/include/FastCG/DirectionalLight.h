#ifndef FASTCG_DIRECTIONAL_LIGHT_H_
#define FASTCG_DIRECTIONAL_LIGHT_H_

#include <FastCG/Light.h>
#include <FastCG/Transform.h>

namespace FastCG
{
	class DirectionalLight : public Light
	{
		DECLARE_COMPONENT(DirectionalLight, Light);

	public:
		inline glm::vec3 GetDirection() const
		{
			return GetGameObject()->GetTransform()->GetPosition();
		}
	};

}

#endif