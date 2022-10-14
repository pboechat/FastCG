#ifndef FASTCG_RENDERER_H
#define FASTCG_RENDERER_H

#include <FastCG/Component.h>

namespace FastCG
{
	class Renderer : public Component
	{
		DECLARE_ABSTRACT_COMPONENT(Renderer, Component);

	public:
		inline void Render()
		{
			if (IsEnabled())
			{
				OnRender();
			}
		}

		inline virtual size_t GetNumberOfDrawCalls() const
		{
			return 1;
		}

		inline virtual size_t GetNumberOfTriangles() const
		{
			return 0;
		}

	protected:
		virtual void OnRender() = 0;
	};

}

#endif