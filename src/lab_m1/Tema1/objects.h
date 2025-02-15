#pragma once

#include <string>
#include <vector>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace objects
{
    Mesh* CreateTriangle(const std::string& name, glm::vec3 firstcorner, glm::vec3 secondcorner, glm::vec3 thirdcorner, float length, glm::vec3 color);
	Mesh* CreateTank(const std::string& name, glm::vec3 leftbottomcorner, float length, glm::vec3 color);
	Mesh* CreateCircle(glm::vec3 center, float radius, int segments, glm::vec3 color, glm::vec3 center_color);
	void UpdateCircle(Mesh* circle, glm::vec3 center, float radius, int segments, glm::vec3 color);
	Mesh* CreateRectangle(glm::vec3 middleBottom, float length, float width, glm::vec3 color);
	void UpdateRectangle(Mesh* rectangle, glm::vec3 middleBottom, float length, float width, glm::vec3 color);
	Mesh* CreateRectangleOutline(const glm::vec3& corner, float length, float height, const glm::vec3& color);
	void UpdateRectangleOutline(Mesh* rectangleOutline, const glm::vec3& corner, float length, float height, const glm::vec3& color);
}
