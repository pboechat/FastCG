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
	mGBufferFBOId(0),
	mSSAOFBOId(0),
	mSSAOBlurFBOId(0),
	mDepthTextureId(0),
	mAmbientTextureId(0),
	mBlurredAmbientTextureId(0),
	mFinalOutputTextureId(0)
{
}

GBuffer::~GBuffer()
{
	DeallocateResources();
}

void GBuffer::AllocateResources()
{
	// create g-buffer fbo
	glGenFramebuffers(1, &mGBufferFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);

	// create textures
	glGenTextures(NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS, mTexturesIds);
	glGenTextures(1, &mDepthTextureId);
	glGenTextures(1, &mFinalOutputTextureId);

	for (unsigned int i = 0 ; i < NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS; i++)
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

	// create final output texture
	glBindTexture(GL_TEXTURE_2D, mFinalOutputTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mFinalOutputTextureId, 0);

	// create ssao fbo
	glGenFramebuffers(1, &mSSAOFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);

	glGenTextures(1, &mAmbientTextureId);

	// create ambient texture
	glBindTexture(GL_TEXTURE_2D, mAmbientTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mAmbientTextureId, 0);

	// create ssao blur fbo
	glGenFramebuffers(1, &mSSAOBlurFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);

	glGenTextures(1, &mBlurredAmbientTextureId);

	// create blurred ambient texture
	glBindTexture(GL_TEXTURE_2D, mBlurredAmbientTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurredAmbientTextureId, 0);

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
	if (mGBufferFBOId != 0)
	{
		glDeleteFramebuffers(1, &mGBufferFBOId);
	}

	if (mSSAOFBOId != 0)
	{
		glDeleteFramebuffers(1, &mSSAOFBOId);
	}

	if (mSSAOBlurFBOId != 0)
	{
		glDeleteFramebuffers(1, &mSSAOBlurFBOId);
	}

	if (mTexturesIds[0] != 0)
	{
		glDeleteTextures(NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS, mTexturesIds);
	}

	if (mDepthTextureId != 0)
	{
		glDeleteTextures(1, &mDepthTextureId);
	}

	if (mAmbientTextureId != 0)
	{
		glDeleteTextures(1, &mAmbientTextureId);
	}

	if (mBlurredAmbientTextureId != 0)
	{
		glDeleteTextures(1, &mBlurredAmbientTextureId);
	}

	if (mFinalOutputTextureId != 0)
	{
		glDeleteTextures(1, &mFinalOutputTextureId);
	}
}

void GBuffer::BindForGBufferDebugging()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);
}

void GBuffer::BindForSSAODebugging()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOBlurFBOId);
}

void GBuffer::StartFrame()
{
	if (mGBufferFBOId == 0)
	{
		AllocateResources();
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
}

void GBuffer::BindForGeometryPass()
{
	static unsigned int pDrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	glDrawBuffers(NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS, pDrawBuffers);
}

void GBuffer::BindForStencilPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
	glDrawBuffer(0);
}

void GBuffer::BindForLightPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT4);

	for (unsigned int i = 0 ; i < NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS; i++) 
	{
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
	}

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mBlurredAmbientTextureId);
}

void GBuffer::BindForSSAOHighFrequencyPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexturesIds[GTT_NORMAL_TEXTURE]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
}

void GBuffer::BindForSSAOBlurPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mAmbientTextureId);
}

void GBuffer::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
}

#endif