#ifndef FASTCG_SHADER_STRUCTURES_H
#define FASTCG_SHADER_STRUCTURES_H

#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <cstdint>

namespace FastCG
{
    static constexpr uint32_t POSITION_SHADER_INPUT_INDEX = 0;
    static constexpr uint32_t NORMAL_SHADER_INPUT_INDEX = 1;
    static constexpr uint32_t UV_SHADER_INPUT_INDEX = 2;
    static constexpr uint32_t TANGENT_SHADER_INPUT_INDEX = 3;
    static constexpr uint32_t COLOR_SHADER_INPUT_INDEX = 4;
    static constexpr uint32_t SCENE_CONSTANTS_SHADER_RESOURCE_INDEX = 0;
    static constexpr uint32_t INSTANCE_CONSTANTS_SHADER_RESOURCE_INDEX = 1;
    static constexpr uint32_t LIGHTING_CONSTANTS_SHADER_RESOURCE_INDEX = 2;
    static constexpr uint32_t MATERIAL_CONSTANTS_SHADER_RESOURCE_INDEX = 0x10;
    static constexpr uint32_t SHADOW_MAP_PASS_CONSTANTS_SHADER_RESOURCE_INDEX = 0;
    static constexpr uint32_t SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_SHADER_RESOURCE_INDEX = 0;

    struct ShadowMapData
    {
        glm::mat4 viewProjection;
        float bias{0.005f};
        float padding[3];
    };

    struct PCSSData
    {
        ShadowMapData shadowMapData;
        float uvScale{0.05f};
        float nearClip;
        int blockerSearchSamples{16};
        int pcfSamples{16};
    };

    constexpr uint32_t MAX_NUM_INSTANCES = 4096;

    namespace ForwardRenderingPath
    {
        struct SceneConstants
        {
            glm::mat4 view;
            glm::mat4 inverseView;
            glm::mat4 projection;
            glm::mat4 inverseProjection;
            glm::vec2 screenSize;
            float pointSize;
            float padding;
        };

        struct InstanceData
        {
            glm::mat4 model;
            glm::mat4 modelView;
            glm::mat4 modelViewInverseTranspose;
            glm::mat4 modelViewProjection;
        };

        struct InstanceConstants
        {
            InstanceData instanceData[MAX_NUM_INSTANCES];
        };

        struct LightingConstants
        {
            glm::vec4 light0Position;
            glm::vec4 light0ViewPosition;
            glm::vec4 light0DiffuseColor;
            glm::vec4 light0SpecularColor;
            float light0Intensity;
            float light0ConstantAttenuation;
            float light0LinearAttenuation;
            float light0QuadraticAttenuation;
            glm::vec4 ambientColor;
            PCSSData pcssData;
        };

    }

    namespace DeferredRenderingPath
    {
        using SceneConstants = ForwardRenderingPath::SceneConstants;
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

    struct ShadowMapPassInstanceData
    {
        glm::mat4 modelViewProjection;
    };

    struct ShadowMapPassConstants
    {
        ShadowMapPassInstanceData instanceData[MAX_NUM_INSTANCES];
    };

    constexpr size_t NUMBER_OF_RANDOM_SAMPLES = 32;
    constexpr uint32_t NOISE_TEXTURE_WIDTH = 4;
    constexpr uint32_t NOISE_TEXTURE_HEIGHT = NOISE_TEXTURE_WIDTH;
    constexpr uint32_t NOISE_TEXTURE_SIZE = NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT;

    struct SSAOHighFrequencyPassConstants
    {
        glm::mat4 projection;
        glm::vec4 randomSamples[NUMBER_OF_RANDOM_SAMPLES]{};
        float radius{0.05f};
        float distanceScale{50.0f};
        float aspectRatio{};
        float tanHalfFov{};
    };

    struct ImGuiConstants
    {
        glm::mat4 projection;
    };
}

#endif