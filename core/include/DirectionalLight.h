#ifndef DIRECTIONALLIGHT_H_
#define DIRECTIONALLIGHT_H_

#include <Light.h>
#include <Transform.h>

COMPONENT(DirectionalLight, Light)
public:
	inline glm::vec3 GetDirection() const
	{
		return GetGameObject()->GetTransform()->GetForward();
	}

};

#endif