#ifndef FASTCG_LINE_RENDERER_H
#define FASTCG_LINE_RENDERER_H

#include <FastCG/Renderer.h>
#include <FastCG/LineStrip.h>

namespace FastCG
{
	class GameObject;

	class LineRenderer : public Renderer
	{
		DECLARE_COMPONENT(LineRenderer, Renderer);

	public:
		inline void SetLineStrip(LineStrip *pLineStrip)
		{
			mpLineStrip = pLineStrip;
		}

		inline const LineStrip *GetLineStrip()
		{
			return mpLineStrip;
		}

		void OnInstantiate() override
		{
			mpLineStrip = 0;
		}

	protected:
		void OnRender() override;

	private:
		LineStrip *mpLineStrip;
	};

}

#endif