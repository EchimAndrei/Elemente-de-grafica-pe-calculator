#pragma once

#include <string>
#include <vector>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace objects2
{
    Mesh* CreateLShape2(const glm::vec3 position, float L, float l, float h, glm::vec3 color);
    Mesh* CreateParalelipiped(const glm::vec3 position, float L, float l, float h, glm::vec3 color);
    Mesh* CreateDrone(const glm::vec3 position, const float lenght);
    Mesh* CreateElice(const glm::vec3 positions, const float length);
    Mesh* CreateLShape1(const glm::vec3 position, float L, float R, float height, glm::vec3 color);
	Mesh* CreateArrow(float L, glm::vec3 color);
    Mesh* createCube(float x, float y, float z, float length, glm::vec3 color);
    Mesh* createTerrain(int m, int n);
    Mesh* createHouseMesh(float x, float y, float z, float width, float height);
    Mesh* createCircle(float radius, float height, glm::vec3 color);
    Mesh* createTreeMesh(float x, float y, float z, float width, float height);
    
}
