#ifndef FASTCG_FONT_H_
#define FASTCG_FONT_H_

#include <FastCG/Texture.h>
#include <FastCG/Shader.h>
#include <FastCG/Material.h>
#include <FastCG/Mesh.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#define STBTT_STATIC
#include <stb_truetype.h>

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace FastCG
{
	class Font
	{
	public:
		Font(const std::string& rFileName, uint32_t size);

		inline uint32_t GetSize() const
		{
			return mSize;
		}
		void DrawString(const std::string& rText, uint32_t x, uint32_t y, const glm::vec4& rColor);

	private:
		std::string mFileName;
		uint32_t mSize;
		std::shared_ptr<Shader> mpFontShader{ nullptr };
		std::unique_ptr<Mesh> mpCharBillboard{ nullptr };
		std::shared_ptr<Texture> mpCharMap{ nullptr };
		std::unique_ptr<stbtt_bakedchar[]> mCharData{ nullptr };

		void AllocateResources();

	};

}

#endif