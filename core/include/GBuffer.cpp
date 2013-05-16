#ifdef USE_PROGRAMMABLE_PIPELINE

#include <GBuffer.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <memory>

GBuffer::GBuffer(unsigned int& rScreenWidth, unsigned int& rScreenHeight) :
	mrScreenWidth(rScreenWidth),
	mrScreenHeight(rScreenHeight),
	mFBOId(0),
	mDepthTextureId(0)
{
}

GBuffer::~GBuffer()
{
	DeallocateResources();
}

void GBuffer::AllocateResources()
{
	// create FBO
	glGenFramebuffers(1, &mFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOId);

	// create textures
	glGenTextures(GBUFFER_NUM_TEXTURES, mTexturesIds);
	glGenTextures(1, &mDepthTextureId);
	glGenTextures(1, &mFinalTextureId);

	for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mTexturesIds[i], 0);
	}

	// create depth texture
	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, mrScreenWidth, mrScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureId, 0);

	// create final texture
	glBindTexture(GL_TEXTURE_2D, mFinalTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mFinalTextureId, 0);

	unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		THROW_EXCEPTION(OpenGLException, "FBO error: 0x%x\n", status);
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GBuffer::DeallocateResources()
{
	if (mFBOId != 0)
	{
		glDeleteFramebuffers(1, &mFBOId);
	}

	if (mTexturesIds[0] != 0)
	{
		glDeleteTextures(GBUFFER_NUM_TEXTURES, mTexturesIds);
	}

	if (mDepthTextureId != 0)
	{
		glDeleteTextures(1, &mDepthTextureId);
	}

	if (mFinalTextureId != 0)
	{
		glDeleteTextures(1, &mFinalTextureId);
	}
}

void GBuffer::StartFrame()
{
	if (mFBOId == 0)
	{
		AllocateResources();
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOId);
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
}

void GBuffer::BindForGeometryPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOId);
	unsigned int pDrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(GBUFFER_NUM_TEXTURES, pDrawBuffers);
}

void GBuffer::BindForStencilPass()
{
	glDrawBuffer(0);
}

void GBuffer::BindForLightPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES; i++) 
	{
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}
}

void GBuffer::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBOId); 
	glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void GBuffer::SetReadBuffer(GBufferTextureType textureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}

#endif