#ifndef FASTCG_SHADER_STRUCTURES_H
#define FASTCG_SHADER_STRUCTURES_H

#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <cstdint>

namespace FastCG
{
    namespace ForwardRenderingPath
    {
        struct SceneConstants
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 inverseProjection;
            float pointSize;
            float padding[3];
        };

        struct InstanceConstants
        {
            glm::mat4 model;
            glm::mat4 modelView;
            glm::mat4 modelViewInverseTranspose;
            glm::mat4 modelViewProjection;
        };

        struct LightingConstants
        {
            glm::vec4 light0Position;
            glm::vec4 light0DiffuseColor;
            glm::vec4 light0SpecularColor;
            float light0Intensity;
            float light0ConstantAttenuation;
            float light0LinearAttenuation;
            float light0QuadraticAttenuation;
            glm::vec4 ambientColor;
        };

    }

    namespace DeferredRenderingPath
    {
        struct SceneConstants
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 inverseProjection;
            glm::vec2 screenSize;
            float aspectRatio;
            float tanHalfFov;
            float pointSize;
            float padding[3];
        };

        using InstanceConstants = ForwardRenderingPath::InstanceConstants;
        using LightingConstants = ForwardRenderingPath::LightingConstants;

    }

    struct MaterialConstants
    {
        glm::vec4 diffuseColor{FastCG::Colors::WHITE};
        glm::vec4 specularColor{FastCG::Colors::WHITE};
        glm::vec2 colorMapTiling{1, 1};
        glm::vec2 bumpMapTiling{1, 1};
        float shininess{1};
        float padding[3];
    };

    constexpr size_t NUMBER_OF_RANDOM_SAMPLES = 32;
    constexpr uint32_t NOISE_TEXTURE_WIDTH = 4;
    constexpr uint32_t NOISE_TEXTURE_HEIGHT = NOISE_TEXTURE_WIDTH;
    constexpr uint32_t NOISE_TEXTURE_SIZE = NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT;

    struct SSAOHighFrequencyPassConstants
    {
        glm::vec4 randomSamples[NUMBER_OF_RANDOM_SAMPLES];
        float radius{0.05f};
        float distanceScale{50.0f};
        float padding[2];
    };

    struct ImGuiConstants
    {
        glm::mat4 projection;
    };
}

#endif