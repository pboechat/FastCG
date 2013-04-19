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
	ShaderPtr mBumpedDiffuseShaderPtr;
	TexturePtr mGrassColorMapTexture;
	TexturePtr mGrassBumpMapTexture;
	TexturePtr mTennisBallColorMapTexture;
	TexturePtr mTennisBallBumpMapTexture;

	TexturePtr LoadPNGAsTexture(const std::string& rFileName);

};

#endif
