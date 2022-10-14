#ifndef FASTCG_POINTS_RENDERER_H_
#define FASTCG_POINTS_RENDERER_H_

#include <FastCG/Renderer.h>
#include <FastCG/Points.h>

namespace FastCG
{
	class PointsRenderer : public Renderer
	{
		DECLARE_COMPONENT(PointsRenderer, Renderer);

	public:
		inline void SetPoints(Points *pPoints)
		{
			mpPoints = pPoints;
		}

		inline Points *GetPoints() const
		{
			return mpPoints;
		}

		void OnInstantiate() override
		{
			mpPoints = 0;
		}

	protected:
		void OnRender() override;

	private:
		Points *mpPoints;
	};

}

#endif