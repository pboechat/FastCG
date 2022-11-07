#ifndef FASTCG_OPENGL_MATERIAL_H
#define FASTCG_OPENGL_MATERIAL_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseMaterial.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLMaterial : public BaseMaterial<OpenGLBuffer, OpenGLShader, OpenGLTexture>
    {
    private:
        OpenGLMaterial(const MaterialArgs &rArgs);
        OpenGLMaterial(const OpenGLMaterial &rOther) = delete;
        OpenGLMaterial(const OpenGLMaterial &&rOther) = delete;
        virtual ~OpenGLMaterial();

        OpenGLMaterial operator=(const OpenGLMaterial &rOther) = delete;

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif