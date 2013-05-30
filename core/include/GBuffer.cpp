#ifndef FIXED_FUNCTION_PIPELINE

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
	mDepthTextureId(0),
	mSSAOTextureId(0),
	mFinalTextureId(0)
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
	glGenTextures(NUMBER_OF_TEXTURES, mTexturesIds);
	glGenTextures(1, &mDepthTextureId);
	glGenTextures(1, &mSSAOTextureId);
	glGenTextures(1, &mFinalTextureId);

	for (unsigned int i = 0 ; i < NUMBER_OF_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mTexturesIds[i], 0);
	}

	// create depth texture
	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, mrScreenWidth, mrScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureId, 0);

	// create SSAO texture
	glBindTexture(GL_TEXTURE_2D, mSSAOTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mSSAOTextureId, 0);

	// create final texture
	glBindTexture(GL_TEXTURE_2D, mFinalTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, mFinalTextureId, 0);

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
		glDeleteTextures(NUMBER_OF_TEXTURES, mTexturesIds);
	}

	if (mDepthTextureId != 0)
	{
		glDeleteTextures(1, &mDepthTextureId);
	}

	if (mSSAOTextureId != 0)
	{
		glDeleteTextures(1, &mSSAOTextureId);
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

	// clear SSAO texture
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClear(GL_COLOR_BUFFER_BIT);

	// clear final texture
	glDrawBuffer(GL_COLOR_ATTACHMENT5);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::BindForDebugging()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBOId);
}

void GBuffer::BindForGeometryPass()
{
	static unsigned int pDrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOId);
	glDrawBuffers(NUMBER_OF_TEXTURES, pDrawBuffers);
}

void GBuffer::BindForStencilPass()
{
	glDrawBuffer(0);
}

void GBuffer::BindForLightPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT5);
	for (unsigned int i = 0 ; i < NUMBER_OF_TEXTURES; i++) 
	{
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
	}
}

void GBuffer::BindForSSAOHighFrequencyPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexturesIds[GTT_NORMAL_TEXTURE]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
}

void GBuffer::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBOId); 
}

void GBuffer::SetReadBuffer(GBufferTextureType textureType)
{
	switch (textureType)
	{
	case GTT_POSITION_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		break;

	case GTT_DIFFUSE_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		break;

	case GTT_NORMAL_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		break;

	case GTT_SPECULAR_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT3);
		break;

	case GTT_SSAO_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT4);
		break;

	case GTT_FINAL_TEXTURE:
		glReadBuffer(GL_COLOR_ATTACHMENT5);
		break;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown gbuffer texture type");
	}
}

#endif