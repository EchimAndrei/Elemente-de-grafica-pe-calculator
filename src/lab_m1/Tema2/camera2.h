#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

namespace implemented
{
    class Camera2
    {
    public:
        Camera2()
        {
            position = glm::vec3(0, 1, 2);
            forward = glm::vec3(0, 0, -8);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
            rotationOX = rotationOY = rotationOZ = 0.0f;
            prev_position = glm::vec3(0,0,0);
        }

        Camera2(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {   
            Set(position, center, up);
        }

        ~Camera2() {}

        float GetRotationOX() const {
            return rotationOX;
        }

        float GetRotationOY() const {
            return rotationOY;
        }

        float GetRotationOZ() const {
            return rotationOZ;
        }

        glm::vec3 GetCameraPosition() const {
            return position;
        }

		void setViewMatrix(glm::mat4 viewMatrix) {
			glm::vec3 center = position + forward;
			glm::vec3 right = glm::cross(up, forward);
			viewMatrix = glm::lookAt(position, center, up);
		}

        // Update camera
        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            position += glm::normalize(forward) * distance;

        }

        void TranslateUpward(float distance)
        {
            position += glm::normalize(up) * distance;

        }

        void TranslateRight(float distance)
        {
            position += glm::normalize(right) * distance;

        }

        void RotateFirstPerson_OX(float angle)
        {
            glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, right) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(aux));
            up = glm::cross(right, forward);

        }

        void RotateFirstPerson_OY(float angle)
        {
            glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(aux));

            aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
            right = glm::normalize(glm::vec3(aux));

            up = glm::cross(right, forward);

        }

        void RotateFirstPerson_OZ(float angle)
        {
            glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
            right = glm::normalize(glm::vec3(aux));

            aux = glm::rotate(glm::mat4(1.f), angle, forward) * glm::vec4(up, 1);
            forward = glm::normalize(glm::vec3(aux));

            up = glm::cross(right, forward);

        }

        void RotateThirdPerson_OX(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OY(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OZ(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);

        }

        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

    public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 prev_position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        // 0 - persp, 1 - ortho
        int type;

    private:
        float rotationOX = 0.0f;
        float rotationOY = 0.0f;
        float rotationOZ = 0.0f;
    };
}   // namespace implemented
