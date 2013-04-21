#ifdef USE_PROGRAMMABLE_PIPELINE

#include <Font.h>
#include <Exception.h>
#include <ShaderRegistry.h>
#include <MathF.h>
#include <Application.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <gl/glew.h>
#include <gl/GL.h>

const unsigned int Font::NUM_CHARS = 128;

Font::Font(const std::string& rFileName, unsigned int size) :
	mFileName(rFileName),
	mSize(size),
	mBillboards(NUM_CHARS),
	mCharactersTexturesIds(NUM_CHARS),
	mSpacings(NUM_CHARS),
	mOffsets(NUM_CHARS)
{
	AllocateResources();
}

Font::~Font()
{
	DeallocateResources();
}

GeometryPtr Font::CreateCharacterBillboard(unsigned int width, unsigned int height, float s, float t)
{
	std::vector<glm::vec3> vertices(4);
	std::vector<glm::vec3> normals(4);
	std::vector<glm::vec2> uvs(4);
	std::vector<unsigned int> indexes(6);

	vertices[0] = glm::vec3(0.0f, height, 0.0f);
	vertices[1] = glm::vec3(0.0f, 0.0f, 0.0f);
	vertices[2] = glm::vec3(width, 0.0f, 0.0f);
	vertices[3] = glm::vec3(width, height, 0.0f);

	normals[0] = glm::vec3(0.0f, 0.0f, 1.0f);
	normals[1] = glm::vec3(0.0f, 0.0f, 1.0f);
	normals[2] = glm::vec3(0.0f, 0.0f, 1.0f);
	normals[3] = glm::vec3(0.0f, 0.0f, 1.0f);

	uvs[0] = glm::vec2(0.0f, 0.0f);
	uvs[1] = glm::vec2(0.0f, t);
	uvs[2] = glm::vec2(s, t);
	uvs[3] = glm::vec2(s, 0.0f);

	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 0;
	indexes[4] = 2;
	indexes[5] = 3;

	return new Geometry(vertices, indexes, normals, uvs, 0);
}

void Font::AllocateResources()
{
	// initilize freetype lib
	FT_Library library;

	if (FT_Init_FreeType(&library))
	{
		THROW_EXCEPTION(Exception, "FT_Init_FreeType failed: %s", mFileName.c_str());
	}

	FT_Face face;

	if (FT_New_Face(library, mFileName.c_str(), 0, &face))
	{
		// probably a problem with the font file
		THROW_EXCEPTION(Exception, "FT_New_Face failed: %s", mFileName.c_str());
	}

	// allocating characters textures
	glGenTextures(NUM_CHARS, &mCharactersTexturesIds[0]);

	// freetype measures font size in terms of 1/64ths of pixels
	FT_Set_Char_Size(face, mSize << 6, mSize << 6, 96, 96);

	mFontShaderPtr = ShaderRegistry::Find("Font");

	for (unsigned char c = 0; c < NUM_CHARS; c++)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_DEFAULT))
		{
			THROW_EXCEPTION(Exception, "FT_Load_Glyph failed: %s", mFileName.c_str());
		}

		FT_Glyph glyph;

		if (FT_Get_Glyph(face->glyph, &glyph))
		{
			THROW_EXCEPTION(Exception, "FT_Get_Glyph failed: %s", mFileName.c_str());
		}

		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

		FT_Bitmap& bitmap = bitmapGlyph->bitmap;

		unsigned int characterWidth = (unsigned int)bitmap.width;
		unsigned int characterHeight = (unsigned int)bitmap.rows;
		unsigned int textureWidth = MathF::UpperPowerOfTwo(characterWidth);
		unsigned int textureHeight = MathF::UpperPowerOfTwo(characterHeight);

		unsigned char* pData = new unsigned char[2 * textureWidth * textureHeight];

		for (unsigned int j = 0; j < textureHeight; j++) 
		{
			for (unsigned int i = 0; i < textureWidth; i++)
			{
				pData[2 * (i + j * textureWidth)] = 255;
				pData[2 * (i + j * textureWidth) + 1] = (i >= characterWidth || j >= characterHeight) ? 0 : bitmap.buffer[i + characterWidth * j];
			}
		}

		// setting character texture data
		glBindTexture(GL_TEXTURE_2D, mCharactersTexturesIds[c]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pData);

		mBillboards[c] = CreateCharacterBillboard(characterWidth, characterHeight, characterWidth / (float)textureWidth, characterHeight / (float)textureHeight);
		mSpacings[c] = face->glyph->advance.x >> 6;
		mOffsets[c] = glm::vec2((float)bitmapGlyph->left, (float)(bitmapGlyph->top - characterHeight));

		delete[] pData;

		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void Font::DeallocateResources()
{
	glDeleteTextures(128, &mCharactersTexturesIds[0]);
}

void Font::DrawText(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, const glm::vec4& rColor)
{
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::ortho(0.0f, (float)Application::GetInstance()->GetScreenWidth(), 0.0f, (float)Application::GetInstance()->GetScreenHeight());

	float scale = size / (float) mSize;

	unsigned int left = (unsigned int)x;
	for (unsigned int i = 0; i < rText.size(); i++)
	{
		unsigned char c = (unsigned char)rText[i];

		glm::vec2 offset = mOffsets[c];
		unsigned int spacing = mSpacings[c];

		GeometryPtr billboardPtr = mBillboards[c];

		glm::mat4 model(1);
		glm::vec2 screenPosition = glm::vec2(left, y) + offset;
		model[3] = glm::vec4(screenPosition, 0.0f, 1.0f);
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		mFontShaderPtr->Bind();
		mFontShaderPtr->SetMat4("_ModelViewProjection", projection * (view * model));
		mFontShaderPtr->SetVec4("color", rColor);
		mFontShaderPtr->SetTexture("font", mCharactersTexturesIds[c], 0);
		billboardPtr->Draw();
		mFontShaderPtr->Unbind();

		left += spacing;
	}
}

#endif