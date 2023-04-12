#include <FastCG/Core/Math.h>

namespace FastCG
{
    std::vector<glm::vec3> MeshUtils::CalculateNormals(const std::vector<glm::vec3> &positions, const std::vector<uint32_t> &indices)
    {
        std::vector<glm::vec3> normals;
        normals.resize(positions.size());
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            uint32_t i1 = indices[i];
            uint32_t i2 = indices[i + 1];
            uint32_t i3 = indices[i + 2];

            const auto &p1 = positions[i1];
            const auto &p2 = positions[i2];
            const auto &p3 = positions[i3];

            auto n = glm::cross(p2 - p1, p3 - p1);
            normals[i1] += n;
            normals[i2] += n;
            normals[i3] += n;
        }

        for (auto &rNormal : normals)
        {
            rNormal = glm::normalize(rNormal);
        }

        return normals;
    }

    std::vector<glm::vec4> MeshUtils::CalculateTangents(const std::vector<glm::vec3> &positions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices)
    {
        // if there's no positions, exit
        if (positions.empty())
        {
            return {};
        }

        // if there's no UV, exit
        if (uvs.empty())
        {
            return {};
        }

        std::vector<glm::vec4> tangents(positions.size());
        std::vector<glm::vec3> tangents1(positions.size());
        std::vector<glm::vec3> tangents2(positions.size());

        for (size_t i = 0; i < indices.size(); i += 3)
        {
            auto i1 = indices[i];
            auto i2 = indices[i + 1];
            auto i3 = indices[i + 2];

            auto &p1 = positions[i1];
            auto &p2 = positions[i2];
            auto &p3 = positions[i3];

            auto &w1 = uvs[i1];
            auto &w2 = uvs[i2];
            auto &w3 = uvs[i3];

            auto x1 = p2.x - p1.x;
            auto x2 = p3.x - p1.x;

            auto y1 = p2.y - p1.y;
            auto y2 = p3.y - p1.y;

            auto z1 = p2.z - p1.z;
            auto z2 = p3.z - p1.z;

            auto s1 = w2.x - w1.x;
            auto s2 = w3.x - w1.x;

            auto t1 = w2.y - w1.y;
            auto t2 = w3.y - w1.y;

            auto r = 1.0f / (s1 * t2 - s2 * t1);

            glm::vec3 sdir((t2 * x1 - t1 * x2) * r,
                           (t2 * y1 - t1 * y2) * r,
                           (t2 * z1 - t1 * z2) * r);

            glm::vec3 tdir((s1 * x2 - s2 * x1) * r,
                           (s1 * y2 - s2 * y1) * r,
                           (s1 * z2 - s2 * z1) * r);

            tangents1[i1] += sdir;
            tangents1[i2] += sdir;
            tangents1[i3] += sdir;

            tangents2[i1] += tdir;
            tangents2[i2] += tdir;
            tangents2[i3] += tdir;
        }

        for (size_t i = 0; i < positions.size(); i++)
        {
            auto &normal = normals[i];
            auto &tangent = tangents1[i];

            // gram-schmidt orthogonalization
            tangents[i] = glm::vec4(glm::normalize(tangent - normal * glm::dot(normal, tangent)), 0.0f);

            // calculate handedness
            tangents[i].w = (glm::dot(glm::cross(normal, tangent), tangents2[i]) < 0.0f) ? -1.0f : 1.0f;
        }

        return tangents;
    }

    AABB MeshUtils::CalculateBounds(const std::vector<glm::vec3> &positions)
    {
        if (positions.empty())
        {
            return {};
        }

        AABB bounds;
        bounds.min.x = bounds.min.y = bounds.min.z = 1e10f;
        bounds.max.x = bounds.max.y = bounds.max.z = -1e10f;

        for (const auto &rPosition : positions)
        {
            bounds.min.x = MathF::Min(bounds.min.x, rPosition.x);
            bounds.min.y = MathF::Min(bounds.min.y, rPosition.y);
            bounds.min.z = MathF::Min(bounds.min.z, rPosition.z);
            bounds.max.x = MathF::Max(bounds.max.x, rPosition.x);
            bounds.max.y = MathF::Max(bounds.max.y, rPosition.y);
            bounds.max.z = MathF::Max(bounds.max.z, rPosition.z);
        }

        return bounds;
    }

}