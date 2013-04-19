#include <PNGLoader.h>
#include <Exception.h>

#include <png.h>
#include <cstdio>
#include <iostream>

void PNGLoader::Load(const std::string& rFileName, unsigned int* pWidth, unsigned int* pHeight, bool* pTransparency, unsigned char** pData)
{
	png_structp pngStruct;
	png_infop pngInfo;
	unsigned int signatureRead = 0;
	int colorType;
	int interlaceType;
	FILE* pFile;

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
	int bitDepth;
	png_get_IHDR(pngStruct, pngInfo, pWidth, pHeight, &bitDepth, &colorType, &interlaceType, 0, 0);

	*pTransparency = (colorType == PNG_COLOR_TYPE_RGB_ALPHA);

	unsigned int rowBytes = png_get_rowbytes(pngStruct, pngInfo);
	*pData = (unsigned char*) malloc(rowBytes * (*pHeight));
	png_bytepp rowPointers = png_get_rows(pngStruct, pngInfo);

	for (unsigned int i = 0; i < (*pHeight); i++)
	{
		// note that png is ordered top to bottom, but OpenGL expect it bottom to top so the order or swapped
		memcpy(*pData + (rowBytes * ((*pHeight) - 1 - i)), (void*) rowPointers[i], rowBytes);
	}

	// clean up after the read, and free any memory allocated
	png_destroy_read_struct(&pngStruct, &pngInfo, 0);

	// close file
	fclose(pFile);
}