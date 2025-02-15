#include "objects2.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;

Mesh* objects2::CreateLShape1(const glm::vec3 position, float L, float l, float h, glm::vec3 color)
{
    float thickness = 2.5;
    float cos45 = sqrt(2) / 2;
    float sin45 = sqrt(2) / 2;

    auto rotate45 = [cos45, sin45](glm::vec3 v) {
        return glm::vec3(
            v.x * cos45 - v.z * sin45,
            v.y,
            v.x * sin45 + v.z * cos45
        );
    };
    vector<VertexFormat> vertices = {
        VertexFormat(position + rotate45(glm::vec3(-L / 2, 0, -l / 2)), color), // 0
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, 0, -l / 2)), color), // 1
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, 0, l / 2)), color), // 2
        VertexFormat(position + rotate45(glm::vec3(-L / 2, 0, l / 2)), color), // 3

        VertexFormat(position + rotate45(glm::vec3(-L / 2, h, -l / 2)), color), // 4
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, h, -l / 2)), color), // 5
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, h, l / 2)), color), // 6
        VertexFormat(position + rotate45(glm::vec3(-L / 2, h, l / 2)), color), // 7

        VertexFormat(position + rotate45(glm::vec3(L / thickness, 0, -l / 2)), color), // 8
        VertexFormat(position + rotate45(glm::vec3(L / 2, 0, -l / 2)), color), // 9
        VertexFormat(position + rotate45(glm::vec3(L / 2, 0, l / 2)), color), // 10
        VertexFormat(position + rotate45(glm::vec3(L / thickness, 0, l / 2)), color), // 11

        VertexFormat(position + rotate45(glm::vec3(L / thickness, h, -l / 2)), color), // 12
        VertexFormat(position + rotate45(glm::vec3(L / 2, h, -l / 2)), color), // 13
        VertexFormat(position + rotate45(glm::vec3(L / 2, h, l / 2)), color), // 14
        VertexFormat(position + rotate45(glm::vec3(L / thickness, h, l / 2)), color), // 15

        VertexFormat(position + rotate45(glm::vec3(-L / thickness, 0, -l / 2)), color), // 16
        VertexFormat(position + rotate45(glm::vec3(L / thickness, 0, -l / 2)), color), // 17
        VertexFormat(position + rotate45(glm::vec3(L / thickness, 0, l / 2)), color), // 18
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, 0, l / 2)), color), // 19

        VertexFormat(position + rotate45(glm::vec3(-L / thickness, h / 4, -l / 2)), color), // 20
        VertexFormat(position + rotate45(glm::vec3(L / thickness, h / 4, -l / 2)), color), // 21
        VertexFormat(position + rotate45(glm::vec3(L / thickness, h / 4, l / 2)), color), // 22
        VertexFormat(position + rotate45(glm::vec3(-L / thickness, h / 4, l / 2)), color) // 23
    };

    vector<unsigned int> indices = {
        0, 1, 5, 0, 5, 4,
        1, 2, 6, 1, 6, 5,
        2, 3, 7, 2, 7, 6,
        0, 1, 3, 1, 2, 3,

        4, 5, 6, 4, 6, 7,

        8, 9, 13, 8, 13, 12,
        9, 10, 14, 9, 14, 13,
        10, 11, 15, 10, 15, 14,
        8, 9, 11, 9, 10, 11,

        12, 13, 14, 12, 14, 15,

        16, 17, 18, 16, 18, 19,

        20, 21, 22, 20, 22, 23,

        0, 4, 7, 0, 7, 3,
        8, 12, 15, 8, 15, 11,
        19, 18, 23, 23, 18, 22,
        16, 17, 21, 21, 20, 16,
        16, 20, 23, 16, 23, 19
    };

    Mesh* lShape = new Mesh("LShape");
    lShape->InitFromData(vertices, indices);
    return lShape;
}

Mesh* objects2::CreateArrow(float L, glm::vec3 color) {
    float headLength = L * 0.3f;
    float bodyLength = L * 0.7f;
    float headRadius = L * 0.1f;
    float bodyRadius = L * 0.05f;

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int segments = 20;
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float nextAngle = 2.0f * M_PI * (i + 1) / segments;

        glm::vec3 bottomCurrent = glm::vec3(bodyRadius * cos(angle), 0, bodyRadius * sin(angle));
        glm::vec3 bottomNext = glm::vec3(bodyRadius * cos(nextAngle), 0, bodyRadius * sin(nextAngle));

        glm::vec3 topCurrent = glm::vec3(bodyRadius * cos(angle), bodyLength, bodyRadius * sin(angle));
        glm::vec3 topNext = glm::vec3(bodyRadius * cos(nextAngle), bodyLength, bodyRadius * sin(nextAngle));

        vertices.push_back(VertexFormat(bottomCurrent, color));
        vertices.push_back(VertexFormat(bottomNext, color));
        vertices.push_back(VertexFormat(topCurrent, color));
        vertices.push_back(VertexFormat(topNext, color));

        int baseIndex = i * 4;
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 3);
    }

    glm::vec3 tip = glm::vec3(0, bodyLength + headLength, 0);
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float nextAngle = 2.0f * M_PI * (i + 1) / segments;

        glm::vec3 baseCurrent = glm::vec3(headRadius * cos(angle), bodyLength, headRadius * sin(angle));
        glm::vec3 baseNext = glm::vec3(headRadius * cos(nextAngle), bodyLength, headRadius * sin(nextAngle));

        vertices.push_back(VertexFormat(baseCurrent, color));
        vertices.push_back(VertexFormat(baseNext, color));
        vertices.push_back(VertexFormat(tip, color));

        int baseIndex = segments * 4 + i * 3;
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
    }

    Mesh* arrow = new Mesh("arrow");
    arrow->InitFromData(vertices, indices);
    return arrow;
}

Mesh* objects2::createCube(float x, float y, float z, float length, glm::vec3 color) {
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    float halfSize = length / 2.0f;
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y, z - halfSize), color)); 
    
    
    
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y, z - halfSize), color)); // 1
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y, z + halfSize), color)); // 2
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y, z + halfSize), color)); // 3
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + length, z - halfSize), color)); // 4
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + length, z - halfSize), color)); // 5
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + length, z + halfSize), color)); // 6
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + length, z + halfSize), color)); // 7

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(7);
    indices.push_back(4);

    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(1);
    indices.push_back(6);
    indices.push_back(2);

    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(0);
    indices.push_back(5);
    indices.push_back(4);

    Mesh* obstacle = new Mesh("obstacle");
    obstacle->InitFromData(vertices, indices);
    obstacle->SetDrawMode(GL_TRIANGLES);
    return obstacle;
}

Mesh* objects2::createTerrain(int m, int n) {
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    float length = 2.0f;
    float width = 2.0f;
    float dx = length / (m - 1);
    float dz = width / (n - 1);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            float x = i * dx - length / 2;
            float z = j * dz - width / 2;
            vertices.push_back(VertexFormat(glm::vec3(x, 0, z)));
        }
    }

    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            int topLeft = i * n + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + n;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* terrain = new Mesh("terrain");
    terrain->InitFromData(vertices, indices);
    terrain->SetDrawMode(GL_TRIANGLES);
    return terrain;
}

Mesh* objects2::createHouseMesh(float x, float y, float z, float width, float height) {
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    float halfSize = width / 2.0f;
    glm::vec3 color = glm::vec3(127 / 225.f, 127 / 225.f, 127 / 225.f);
    glm::vec3 darkerColor = glm::vec3(90 / 225.f, 90 / 225.f, 90 / 225.f);
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y, z - halfSize), color)); // 0
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y, z - halfSize), color)); // 1
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y, z + halfSize), color)); // 2
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y, z + halfSize), color)); // 3
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + width, z - halfSize), darkerColor)); // 4
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + width, z - halfSize), darkerColor)); // 5
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + width, z + halfSize), darkerColor)); // 6
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + width, z + halfSize), darkerColor)); // 7


    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(7);
    indices.push_back(4);

    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(1);
    indices.push_back(6);
    indices.push_back(2);

    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(0);
    indices.push_back(5);
    indices.push_back(4);


    vertices.push_back(VertexFormat(glm::vec3(x, y + height, z), glm::vec3(90 / 225.f, 0, 0)));
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + width, z - halfSize), glm::vec3(50 / 225.f, 0, 0)));
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + width, z - halfSize), glm::vec3(50 / 225.f, 0, 0)));
    vertices.push_back(VertexFormat(glm::vec3(x + halfSize, y + width, z + halfSize), glm::vec3(50 / 225.f, 0, 0)));
    vertices.push_back(VertexFormat(glm::vec3(x - halfSize, y + width, z + halfSize), glm::vec3(50 / 225.f, 0, 0)));

    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(10);

    indices.push_back(8);
    indices.push_back(10);
    indices.push_back(11);

    indices.push_back(8);
    indices.push_back(11);
    indices.push_back(12);

    indices.push_back(8);
    indices.push_back(12);
    indices.push_back(9);


    Mesh* house = new Mesh("house");
    house->InitFromData(vertices, indices);
    house->SetDrawMode(GL_TRIANGLES);

    return house;
}

Mesh* objects2::createCircle(float radius, float height, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int numSegments = 100;
    float angleStep = 2.0f * glm::pi<float>() / numSegments;

    for (int i = 0; i <= numSegments; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(glm::vec3(x, 0, z), color);
        vertices.emplace_back(glm::vec3(x, height, z), color);
    }

    for (int i = 0; i < numSegments; i++) {
        unsigned int bottom1 = i * 2;
        unsigned int bottom2 = (i + 1) * 2 % (2 * numSegments);
        unsigned int top1 = bottom1 + 1;
        unsigned int top2 = bottom2 + 1;

        indices.push_back(bottom1);
        indices.push_back(top1);
        indices.push_back(bottom2);

        indices.push_back(top1);
        indices.push_back(top2);
        indices.push_back(bottom2);
    }

    for (int i = 0; i < numSegments; i++) {
        indices.push_back(0);
        indices.push_back(i * 2);
        indices.push_back((i + 1) * 2 % (2 * numSegments));
    }

    for (int i = 0; i < numSegments; i++) {
        indices.push_back(1);
        indices.push_back((i * 2 + 1));
        indices.push_back(((i + 1) * 2 + 1) % (2 * numSegments));
    }

    Mesh* circlePlatform = new Mesh("circlePlatform");
    circlePlatform->InitFromData(vertices, indices);
    return circlePlatform;
}

Mesh* objects2::createTreeMesh(float x, float y, float z, float width, float height) {
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    float radius = width / 2.f;
    float cylinderHeight = height / 3.f;
    int numTriangles = 50;
    float angle = 2 * M_PI / numTriangles;

    for (int i = 0; i < numTriangles; i++) {
        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos(i * angle), y, z + radius * sin(i * angle)), glm::vec3(0.4, 0.25, 0.05)));
        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos((i + 1) * angle), y, z + radius * sin((i + 1) * angle)), glm::vec3(0.4, 0.25, 0.05)));
        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos(i * angle), y + cylinderHeight, z + radius * sin(i * angle)), glm::vec3(0.5, 0.35, 0.05)));

        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos(i * angle), y + cylinderHeight, z + radius * sin(i * angle)), glm::vec3(0.4, 0.25, 0.05)));
        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos((i + 1) * angle), y, z + radius * sin((i + 1) * angle)), glm::vec3(0.4, 0.3, 0.05)));
        vertices.push_back(VertexFormat(glm::vec3(x + radius * cos((i + 1) * angle), y + cylinderHeight, z + radius * sin((i + 1) * angle)), glm::vec3(0.5, 0.35, 0.05)));

        indices.push_back(i * 6);
        indices.push_back(i * 6 + 1);
        indices.push_back(i * 6 + 2);
        indices.push_back(i * 6 + 3);
        indices.push_back(i * 6 + 4);
        indices.push_back(i * 6 + 5);
    }

    float coneRadius = width;
    float coneHeight = height / 3.f;
    for (int i = 0; i < numTriangles; i++) {
        vertices.push_back(VertexFormat(glm::vec3(x + coneRadius * cos(i * angle), y + cylinderHeight, z + coneRadius * sin(i * angle)), glm::vec3(0, 0.4, 0)));
        vertices.push_back(VertexFormat(glm::vec3(x + coneRadius * cos((i + 1) * angle), y + cylinderHeight, z + coneRadius * sin((i + 1) * angle)), glm::vec3(0, 0.4, 0)));
        vertices.push_back(VertexFormat(glm::vec3(x, y + coneHeight + cylinderHeight, z), glm::vec3(0, 0.7, 0)));

        indices.push_back(numTriangles * 6 + i * 3);
        indices.push_back(numTriangles * 6 + i * 3 + 1);
        indices.push_back(numTriangles * 6 + i * 3 + 2);
    }

    float cone2Radius = width;
    float cone2Height = height / 3.f;
    for (int i = 0; i < numTriangles; i++) {
        vertices.push_back(VertexFormat(glm::vec3(x + cone2Radius * cos(i * angle), y + cylinderHeight + coneHeight / 1.5f, z + cone2Radius * sin(i * angle)), glm::vec3(0, 0.4, 0)));
        vertices.push_back(VertexFormat(glm::vec3(x + cone2Radius * cos((i + 1) * angle), y + cylinderHeight + coneHeight / 1.5f, z + cone2Radius * sin((i + 1) * angle)), glm::vec3(0, 0.4, 0)));
        vertices.push_back(VertexFormat(glm::vec3(x, y + coneHeight + cylinderHeight + cone2Height, z), glm::vec3(0, 0.7, 0)));

        indices.push_back(numTriangles * 9 + i * 3);
        indices.push_back(numTriangles * 9 + i * 3 + 1);
        indices.push_back(numTriangles * 9 + i * 3 + 2);
    }

    Mesh* tree = new Mesh("tree");
    tree->InitFromData(vertices, indices);
    tree->SetDrawMode(GL_TRIANGLES);
    return tree;
}

Mesh* objects2::CreateLShape2(const glm::vec3 position, float L, float l, float h, glm::vec3 color)
{
    float thickness = 2.5;
    float cos45 = sqrt(2) / 2;
    float sin45 = sqrt(2) / 2;

    auto rotate45 = [cos45, sin45](glm::vec3 v) {
        return glm::vec3(
            v.x * cos45 - v.z * sin45,
            v.y,
            v.x * sin45 + v.z * cos45
        );
        };

    vector<VertexFormat> vertices = {
        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, -L / 2)), color),             // 0
        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, -L / thickness)), color), // 1
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, -L / thickness)), color),  // 2
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, -L / 2)), color),              // 3

        VertexFormat(position + rotate45(glm::vec3(-l / 2, h, -L / 2)), color),             // 4
        VertexFormat(position + rotate45(glm::vec3(-l / 2, h, -L / thickness)), color), // 5
        VertexFormat(position + rotate45(glm::vec3(l / 2, h, -L / thickness)), color),  // 6
        VertexFormat(position + rotate45(glm::vec3(l / 2, h, -L / 2)), color),              // 7

        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, L / thickness)), color),  // 8
        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, L / 2)), color),              // 9
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, L / 2)), color),               // 10
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, L / thickness)), color),   // 11

        VertexFormat(position + rotate45(glm::vec3(-l / 2, h, L / thickness)), color),  // 12
        VertexFormat(position + rotate45(glm::vec3(-l / 2, h, L / 2)), color),              // 13
        VertexFormat(position + rotate45(glm::vec3(l / 2, h, L / 2)), color),               // 14
        VertexFormat(position + rotate45(glm::vec3(l / 2, h, L / thickness)), color),   // 15

        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, -L / thickness)), color), // 16
        VertexFormat(position + rotate45(glm::vec3(-l / 2, 0, L / thickness)), color),  // 17
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, L / thickness)), color),   // 18
        VertexFormat(position + rotate45(glm::vec3(l / 2, 0, -L / thickness)), color),  // 19

        VertexFormat(position + rotate45(glm::vec3(-l / 2, h / 4, -L / thickness)), color), // 20
        VertexFormat(position + rotate45(glm::vec3(-l / 2, h / 4, L / thickness)), color),  // 21
        VertexFormat(position + rotate45(glm::vec3(l / 2, h / 4, L / thickness)), color),   // 22
        VertexFormat(position + rotate45(glm::vec3(l / 2, h / 4, -L / thickness)), color)   // 23
    };

    vector<unsigned int> indices = {
        0, 1, 5, 0, 5, 4,
        1, 2, 6, 1, 6, 5,
        2, 3, 7, 2, 7, 6,
        0, 1, 3, 1, 2, 3,

        4, 5, 6, 4, 6, 7,

        8, 9, 13, 8, 13, 12,
        9, 10, 14, 9, 14, 13,
        10, 11, 15, 10, 15, 14,
        8, 9, 11, 9, 10, 11,

        12, 13, 14, 12, 14, 15,

        16, 17, 18, 16, 18, 19,

        20, 21, 22, 20, 22, 23,

        0, 4, 7, 0, 7, 3,
        8, 12, 15, 8, 15, 11,
        19, 18, 23, 23, 18, 22,
        16, 17, 21, 21, 20, 16,
        16, 20, 23, 16, 23, 19
    };

    Mesh* lShape = new Mesh("LShape");
    lShape->InitFromData(vertices, indices);
    return lShape;
}

Mesh* objects2::CreateParalelipiped(const glm::vec3 position, float L, float l, float h, glm::vec3 color) {
	vector<VertexFormat> vertices = {
		VertexFormat(position + glm::vec3(-L / 2, 0, -l / 2), color), // 0
		VertexFormat(position + glm::vec3(L / 2, 0, -l / 2), color),  // 1
		VertexFormat(position + glm::vec3(L / 2, 0, l / 2), color),   // 2
		VertexFormat(position + glm::vec3(-L / 2, 0, l / 2), color),  // 3

		VertexFormat(position + glm::vec3(-L / 2, h, -l / 2), color), // 4
		VertexFormat(position + glm::vec3(L / 2, h, -l / 2), color),  // 5
		VertexFormat(position + glm::vec3(L / 2, h, l / 2), color),   // 6
		VertexFormat(position + glm::vec3(-L / 2, h, l / 2), color),  // 7
	};

	vector<unsigned int> indices = {
		0, 1, 2, 0, 2, 3,

		4, 5, 6, 4, 6, 7,

		0, 4, 7, 0, 7, 3,
		1, 5, 6, 1, 6, 2,
		3, 2, 6, 3, 6, 7,
		0, 1, 5, 0, 5, 4,
	};

	Mesh* paralelipiped = new Mesh("Paralelipiped");
	paralelipiped->InitFromData(vertices, indices);
    return paralelipiped;
}

Mesh* objects2::CreateDrone(const glm::vec3 position, const float length)
{
    float L = length;
    float l = length / 8;
    float h = length / 8;

    Mesh* LShape1 = CreateLShape1(position, L, l, h, glm::vec3(0.5, 0.5, 0.5));
    Mesh* LShape2 = CreateLShape2(position, L, l, h, glm::vec3(0.5, 0.5, 0.5));

    vector<VertexFormat> vertices;
    for (const auto& vertex : LShape1->vertices)
    {
		vertices.emplace_back(vertex);
    }

    for (const auto& vertex : LShape2->vertices)
    {
		vertices.emplace_back(vertex);
    }

    vector<unsigned int> indices = LShape1->indices;
    for (const auto& index : LShape2->indices)
    {
        indices.push_back(index + LShape1->vertices.size());
    }

    Mesh* drone = new Mesh("Drone");
    drone->InitFromData(vertices, indices);
    return drone;
}

Mesh* objects2::CreateElice(const glm::vec3 position, const float length) {

    float eliceLenght = length / 3;
    float eliceHeight = length / 16;
    float eliceWidth = length / 20;
    Mesh* paralelipiped = CreateParalelipiped(position, eliceLenght, eliceWidth, eliceHeight, glm::vec3(0,0,1));

    vector<VertexFormat> vertices = paralelipiped->vertices;
    vector<unsigned int> indices = paralelipiped->indices;

	Mesh* elice = new Mesh("Elice");
	elice->InitFromData(vertices, indices);
	return elice;
}