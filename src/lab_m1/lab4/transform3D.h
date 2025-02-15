#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        // TODO(student): Implement the translation matrix
        return glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            translateX, translateY, translateZ, 1.f
        );

    }

    // Scale matrix
    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        // TODO(student): Implement the scaling matrix
        return glm::mat4(
            scaleX, 0.f, 0.f, 0.f,
            0.f, scaleY, 0.f, 0.f,
            0.f, 0.f, scaleZ, 0.f,
            0.f, 0.f, 0.f, 1.f
        );

    }

    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            cos(radians), sin(radians), 0.f, 0.f,
            -sin(radians), cos(radians), 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        );

    }

    // Rotate matrix relative to the OY axis
    inline glm::mat4 RotateOY(float radians)
    {
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            cos(radians), 0.f, -sin(radians), 0.f,
            0.f, 1.f, 0.f, 0.f,
            sin(radians), 0.f, cos(radians), 0.f,
            0.f, 0.f, 0.f, 1.f
        );

    }

    // Rotate matrix relative to the OX axis
    inline glm::mat4 RotateOX(float radians)
    {
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, cos(radians), sin(radians), 0.f,
            0.f, -sin(radians), cos(radians), 0.f,
            0.f, 0.f, 0.f, 1.f
        );

    }
}   // namespace transform3D
