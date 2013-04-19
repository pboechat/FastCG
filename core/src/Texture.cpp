#include <Texture.h>
#include <Exception.h>

#include <cstdio>
#include <png.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Texture::Texture(unsigned int width, unsigned int height, TextureFormat format, DataType dataType, FilterMode filter, WrapMode wrapMode, void* pData) :
	mWidth(width), mHeight(height), mFormat(format), mDataType(dataType), mFilter(filter), mWrapMode(wrapMode), mpData(pData)
{
	AllocateResources();
}

Texture::~Texture()
{
	DeallocateResources();
}

Pointer<Texture> Texture::LoadPNG(const std::string& rFileName)
{
	png_structp pngStruct;
	png_infop pngInfo;
	unsigned int signatureRead = 0;
	int colorType;
	int interlaceType;
	FILE* pFile;
	unsigned char* pData;

	if ((pFile = fopen(rFileName.c_str(), "rb")) == 0)
	{
		// TODO:
		THROW_EXCEPTION(Exception, "Error loading PNG: %d", 0);
	}

	// create and initialize the png_struct with the desired error handler functions.
	// if you want to use the default stderr and longjump method,
	// you can supply NULL for the last three parameters.
	// we also supply the the compiler header file version,
	// so that we know if the application was compiled with a compatible version of the library.
	// REQUIRED
	pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if (pngStruct == 0)
	{
		fclose(pFile);
		// TODO:
		THROW_EXCEPTION(Exception, "Error loading PNG: %d", 0);
	}

	// allocate/initialize the memory for image information.
	// REQUIRED.
	pngInfo = png_create_info_struct(pngStruct);

	if (pngInfo == 0)
	{
		fclose(pFile);
		png_destroy_read_struct(&pngStruct, NULL, NULL);
		// TODO:
		THROW_EXCEPTION(Exception, "Error loading PNG: %d", 0);
	}

	// set error handling if you are using the setjmp/longjmp method
	// (this is the normal method of doing things with libpng).
	// REQUIRED unless you  set up your own error handlers in the png_create_read_struct() earlier.
	if (setjmp(png_jmpbuf(pngStruct)))
	{
		// free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&pngStruct, &pngInfo, NULL);
		fclose(pFile);
		// if we get here, we had a problem reading the file
		// TODO:
		THROW_EXCEPTION(Exception, "Error loading PNG: %d", 0);
	}

	// set up the output control if you are using standard C streams
	png_init_io(pngStruct, pFile);
	// if we have already read some of the signature
	png_set_sig_bytes(pngStruct, signatureRead);
	// if you have enough memory to read in the entire image at once,
	// and you need to specify only transforms that can be controlled with one of the PNG_TRANSFORM_bits
	// (this presently excludes dithering, filling, setting background, and doing gamma adjustment),
	// then you can read the entire image (including pixels) into the info structure with this call
	// PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING
	// forces 8 bit PNG_TRANSFORM_EXPAND forces to expand a palette into RGB
	png_read_png(pngStruct, pngInfo, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	unsigned int width;
	unsigned int height;
	int bitDepth;
	png_get_IHDR(pngStruct, pngInfo, &width, &height, &bitDepth, &colorType, &interlaceType, 0, 0);
	unsigned int rowBytes = png_get_rowbytes(pngStruct, pngInfo);
	pData = (unsigned char*) malloc(rowBytes * height);
	png_bytepp rowPointers = png_get_rows(pngStruct, pngInfo);

	for (unsigned int i = 0; i < height; i++)
	{
		// note that png is ordered top to bottom, but OpenGL expect it bottom to top so the order or swapped
		memcpy(pData + (rowBytes * (height - 1 - i)), (void*) rowPointers[i], rowBytes);
	}

	// clean up after the read, and free any memory allocated
	png_destroy_read_struct(&pngStruct, &pngInfo, 0);
	// close file
	fclose(pFile);
	return new Texture(width, height, ((colorType == PNG_COLOR_TYPE_RGB_ALPHA) ? TF_RGBA : TF_RGB), DT_UNSIGNED_CHAR, FM_BILINEAR, WM_CLAMP, pData);
}

void Texture::AllocateResources()
{
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	SetUpFilter();
	SetUpWrapping();

	if (mDataType == DT_UNSIGNED_CHAR)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormatMapping(), mWidth, mHeight, 0, GetFormatMapping(), GetDataTypeMapping(), mpData);

	if (mFilter == FM_TRILINEAR)
	{
		GenerateMipmaps();
	}

	// TODO: check allocation errors!
}

void Texture::DeallocateResources()
{
	glDeleteTextures(1, &mTextureId);
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
}

void Texture::SetUpFilter() const
{
	switch (mFilter)
	{
	case FM_POINT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case FM_BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case FM_TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown filter mode: %d", mFilter)
		;
	}
}

void Texture::SetUpWrapping() const
{
	switch (mWrapMode)
	{
	case WM_CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;

	case WM_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown wrapping mode: %d", mWrapMode)
		;
	}
}

unsigned int Texture::GetInternalFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;

	case TF_RGBA:
		return GL_RGBA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat)
		;
		return 0;
	}
}

unsigned int Texture::GetFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;

	case TF_RGBA:
		return GL_RGBA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat)
		;
		return 0;
	}
}

unsigned int Texture::GetDataTypeMapping() const
{
	switch (mDataType)
	{
	case DT_FLOAT:
		return GL_FLOAT;

	case DT_UNSIGNED_CHAR:
		return GL_UNSIGNED_BYTE;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown data type: %d", mDataType)
		;
		return 0;
	}
}

void Texture::GenerateMipmaps() const
{
	glGenerateMipmap(GL_TEXTURE_2D);
}
