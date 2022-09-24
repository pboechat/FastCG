#include <FastCG/ShaderRegistry.h>
#include <FastCG/MathT.h>
#include <FastCG/Font.h>
#include <FastCG/FileReader.h>
#include <FastCG/Exception.h>
#include <FastCG/Application.h>

#include <glm/gtc/matrix_transform.hpp>

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <memory>

namespace FastCG
{
	constexpr uint32_t NUM_CHARS = 96;
	constexpr uint32_t BITMAP_SIDE = 512;

	Font::Font(const std::string& rFileName, uint32_t size) :
		mFileName(rFileName),
		mSize(size)
	{
		AllocateResources();
	}

	void Font::AllocateResources()
	{
		mCharData = std::make_unique<stbtt_bakedchar[]>(NUM_CHARS);

		{
			size_t fileSize;
			auto data = FileReader::ReadBinary(mFileName.c_str(), fileSize);
			if (data == nullptr)
			{
				FASTCG_THROW_EXCEPTION(Exception, "Failed to load font: %s", mFileName.c_str());
			}

			{
				auto bitmap = std::make_unique<uint8_t[]>(BITMAP_SIDE * BITMAP_SIDE);
				auto res = stbtt_BakeFontBitmap(data.get(), 0, (float)mSize, bitmap.get(), BITMAP_SIDE, BITMAP_SIDE, 32, NUM_CHARS, mCharData.get());

				if (res == -1)
				{
					FASTCG_THROW_EXCEPTION(Exception, "Failed to bake font bitmap: %s", mFileName.c_str());
				}

				mpCharMap = std::make_shared<Texture>(
					mFileName,
					BITMAP_SIDE,
					BITMAP_SIDE,
					TextureFormat::TF_R,
					TextureDataType::DT_UNSIGNED_CHAR,
					TextureFilter::TF_LINEAR_FILTER,
					TextureWrapMode::TW_CLAMP,
					true,
					bitmap.get()
					);
			}
		}

		mpFontShader = ShaderRegistry::Find("Font");

		mpBillboard = std::unique_ptr<Mesh>(new Mesh(mFileName + " Billboard", { 0, 1, 3, 0, 3, 2 }));
	}

	void Font::DrawString(const std::string& rText, uint32_t x, uint32_t y, const glm::vec4& rColor)
	{
		auto projection = glm::ortho(0.0f, (float)Application::GetInstance()->GetScreenWidth(), 0.0f, (float)Application::GetInstance()->GetScreenHeight(), 0.0f, 1.0f);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_STENCIL_TEST);
		glStencilMask(0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		auto nX = (float)x;
		auto nY = (float)y;
		auto baseLine = 2 * y - mSize;
		mpFontShader->Bind();
		mpFontShader->SetVec2("_ScreenSize", glm::vec2{ (float)Application::GetInstance()->GetScreenWidth(), (float)Application::GetInstance()->GetScreenHeight() });
		mpFontShader->SetVec4("_Color", rColor);
		mpFontShader->SetTexture("_Map", mpCharMap, 0);
		for (const auto& c : rText)
		{
			if (c < 32 || c > 128)
			{
				continue;
			}

			stbtt_aligned_quad quad;
			stbtt_GetBakedQuad(mCharData.get(), BITMAP_SIDE, BITMAP_SIDE, c - 32, &nX, &nY, &quad, 1);

			mpFontShader->SetVec4("_ScreenCoords", glm::vec4{ quad.x0, baseLine - quad.y1, quad.x1 - quad.x0, quad.y1 - quad.y0 });
			mpFontShader->SetVec4("_MapCoords", glm::vec4{ quad.s0, quad.t1, quad.s1 - quad.s0, quad.t0 - quad.t1 });
			mpBillboard->Draw();
		}
		mpFontShader->Unbind();
	}

}