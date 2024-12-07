#ifndef FASTCG_SHADER_STRUCTURES_H
#define FASTCG_SHADER_STRUCTURES_H

#include <FastCG/Core/Colors.h>

#include <glm/glm.hpp>

#include <cstdint>

namespace FastCG
{
    static constexpr uint32_t POSITION_VERTEX_INPUT_LOCATION = 0;
    static constexpr uint32_t NORMAL_VERTEX_INPUT_LOCATION = 1;
    static constexpr uint32_t UV_VERTEX_INPUT_LOCATION = 2;
    static constexpr uint32_t TANGENT_VERTEX_INPUT_LOCATION = 3;
    static constexpr uint32_t COLOR_SHADER_INPUT_INDEX = 4;
    static constexpr const char *const SCENE_CONSTANTS_SHADER_RESOURCE_NAME = "SceneConstants";
    static constexpr const char *const INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME = "InstanceConstants";
    static constexpr const char *const FOG_CONSTANTS_SHADER_RESOURCE_NAME = "FogConstants";
    static constexpr const char *const PCSS_CONSTANTS_SHADER_RESOURCE_NAME = "PCSSConstants";
    static constexpr const char *const LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME = "LightingConstants";
    static constexpr const char *const MATERIAL_CONSTANTS_SHADER_RESOURCE_NAME = "MaterialConstants";
    static constexpr const char *const SHADOW_MAP_PASS_CONSTANTS_SHADER_RESOURCE_NAME = "ShadowMapPassConstants";
    static constexpr const char *const SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_SHADER_RESOURCE_NAME =
        "SSAOHighFrequencyPassConstants";

    struct ShadowMapData
    {
        glm::mat4 viewProjection;
        float bias{0.0025f};
        float padding[3];
    };

    struct FogData
    {
        glm::vec4 color;
        uint32_t mode;
        float density;
        float start;
        float end;
    };

    struct FogConstants
    {
        FogData fogData;
    };

    struct PCSSData
    {
        ShadowMapData shadowMapData;
        float uvScale
        {
            // TODO: solve this in a more elegant way
#if FASTCG_VULKAN
            0.001f
#else
            0.05f
#endif
        };
        float nearClip;
        int blockerSearchSamples{16};
        int pcfSamples{16};
    };

    struct PCSSConstants
    {
        PCSSData pcssData;
    };

    constexpr uint32_t MAX_NUM_INSTANCES = 256;

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
        };

        struct InstanceData
        {
            glm::mat4 model;
            glm::mat4 modelInverseTranspose;
            glm::mat4 viewProjection;
            glm::mat4 modelViewProjection;
        };

        struct InstanceConstants
        {
            InstanceData instanceData[MAX_NUM_INSTANCES];
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
        using SceneConstants = ForwardRenderingPath::SceneConstants;
        using InstanceConstants = ForwardRenderingPath::InstanceConstants;
        using LightingConstants = ForwardRenderingPath::LightingConstants;

    }

    struct ShadowMapPassInstanceData
    {
        glm::mat4 modelViewProjection;
    };

    struct ShadowMapPassConstants
    {
        ShadowMapPassInstanceData instanceData[MAX_NUM_INSTANCES];
    };

    constexpr size_t NUMBER_OF_RANDOM_SAMPLES = 64;
    constexpr uint32_t NOISE_TEXTURE_WIDTH = 4;
    constexpr uint32_t NOISE_TEXTURE_HEIGHT = NOISE_TEXTURE_WIDTH;

    struct SSAOHighFrequencyPassConstants
    {
        glm::mat4 projection;
        glm::vec4 randomSamples[NUMBER_OF_RANDOM_SAMPLES]{};
        float radius{0.5f};
        float bias{0.0025f};
        float aspectRatio{};
        float tanHalfFov{};
    };

    struct ImGuiConstants
    {
        glm::mat4 projection;
    };
}

#endif