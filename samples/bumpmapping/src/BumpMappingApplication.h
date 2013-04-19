#ifndef BUMPMAPPINGAPPLICATION_H_
#define BUMPMAPPINGAPPLICATION_H_

#include <Application.h>
#include <Shader.h>
#include <Texture.h>

#include <string>

class BumpMappingApplication: public Application
{
public:
	BumpMappingApplication();
	virtual ~BumpMappingApplication();

protected:
	virtual void OnStart();

private:
	static const unsigned int FIELD_SIZE;
	static const unsigned int NUM_SPHERE_SEGMENTS;

	ShaderPtr mBumpedDiffuseShaderPtr;
	TexturePtr mGrassColorMapTexturePtr;
	TexturePtr mGrassBumpMapTexturePtr;
	TexturePtr mTennisBallColorMapTexturePtr;
	TexturePtr mTennisBallBumpMapTexturePtr;

	TexturePtr LoadPNGAsTexture(const std::string& rFileName);

};

#endif
