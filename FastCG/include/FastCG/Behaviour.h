#ifndef FASTCG_BEHAVIOUR_H_
#define FASTCG_BEHAVIOUR_H_

#include <FastCG/Component.h>

namespace FastCG
{
	class Behaviour : public Component
	{
		DECLARE_ABSTRACT_COMPONENT(Behaviour, Component);

	public:
		inline void Update(float time, float deltaTime)
		{
			if (IsEnabled())
			{
				OnUpdate(time, deltaTime);
			}
		}

	protected:
		virtual void OnUpdate(float time, float deltaTime) = 0;

	};

}

#endif