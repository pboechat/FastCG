#ifndef FASTCG_DIRECTIONAL_LIGHT_H
#define FASTCG_DIRECTIONAL_LIGHT_H

#include <FastCG/World/Transform.h>
#include <FastCG/Rendering/Light.h>

namespace FastCG
{
	class DirectionalLight : public Light
	{
		FASTCG_DECLARE_COMPONENT(DirectionalLight, Light);

	public:
		inline glm::vec3 GetDirection() const
		{
			return glm::normalize(-GetGameObject()->GetTransform()->GetWorldPosition());
		}
	};

}

#endif