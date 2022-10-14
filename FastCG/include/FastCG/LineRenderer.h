#ifndef FASTCG_LINE_RENDERER_H
#define FASTCG_LINE_RENDERER_H

#include <FastCG/Renderer.h>
#include <FastCG/LineStrip.h>

namespace FastCG
{
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

		inline void OnInstantiate() override
		{
		}

	protected:
		inline void OnRender() override
		{
			if (mpLineStrip == nullptr)
			{
				return;
			}

			mpLineStrip->Draw();
		}

	private:
		LineStrip *mpLineStrip{nullptr};
	};

}

#endif