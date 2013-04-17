#ifndef BUMPMAPPINGAPPLICATION_H_
#define BUMPMAPPINGAPPLICATION_H_

#include <Application.h>
#include <Shader.h>
#include <Texture.h>

class BumpMappingApplication: public Application
{
public:
	BumpMappingApplication();
	virtual ~BumpMappingApplication();

protected:
	virtual bool OnStart();

private:
	ShaderPtr mBumpedDiffuseShaderPtr;
	TexturePtr mGrassColorMapTexture;
	TexturePtr mGrassBumpMapTexture;
	TexturePtr mTennisBallColorMapTexture;
	TexturePtr mTennisBallBumpMapTexture;

};

#endif
