#include "objects.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;

Mesh* objects::CreateTriangle(
    const string& name,
    glm::vec3 firstcorner,
    glm::vec3 secondcorner,
    glm::vec3 thirdcorner,
    float length,
    glm::vec3 color)
{

    vector<VertexFormat> vertices =
    {
        VertexFormat(firstcorner, color),
        VertexFormat(secondcorner, color),
        VertexFormat(thirdcorner, color)
    };

    Mesh* triangle = new Mesh(name);
    vector<unsigned int> indices = { 0, 1, 2, };

    triangle->InitFromData(vertices, indices);
    return triangle;
}

Mesh* objects::CreateTank(
    const string& name,
    glm::vec3 leftbottomcorner,
    float length,
    glm::vec3 color)
{
    glm::vec3 corner = leftbottomcorner;
    corner.x -= length / 2.0f;

    float height_first_trapez = length / 4.0f;
    glm::vec3 dark_color = color - glm::vec3(70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f);

    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    vertices.push_back(VertexFormat(corner, dark_color));
    vertices.push_back(VertexFormat(corner + glm::vec3(length, 0, 0), dark_color));
    vertices.push_back(VertexFormat(corner + glm::vec3(length + length / 4.0f, height_first_trapez, 0), dark_color));
    vertices.push_back(VertexFormat(corner + glm::vec3(-length / 4.0f, height_first_trapez, 0), dark_color));

    unsigned int base_indices[] = { 0, 1, 2, 0, 2, 3 };
    indices.insert(indices.end(), begin(base_indices), end(base_indices));

    float top_height = height_first_trapez + length / 3.0f;
    vertices.push_back(VertexFormat(corner + glm::vec3(-length / 2.0f, height_first_trapez, 0), color));
    vertices.push_back(VertexFormat(corner + glm::vec3(length + length / 2.0f, height_first_trapez, 0), color));
    vertices.push_back(VertexFormat(corner + glm::vec3(length + length / 4.0f, top_height, 0), color));
    vertices.push_back(VertexFormat(corner + glm::vec3(-length / 4.0f, top_height, 0), color));

    unsigned int top_indices[] = { 4, 5, 6, 4, 6, 7 };
    indices.insert(indices.end(), begin(top_indices), end(top_indices));

    float circle_radius = length / 3.0f;
    glm::vec3 circle_center = corner + glm::vec3(length / 2.0f, top_height, 0);
    int num_segments = 50;

    vertices.push_back(VertexFormat(circle_center, color));
    unsigned int center_index = vertices.size() - 1;

    for (int i = 0; i <= num_segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments);
        float dx = circle_radius * cosf(theta);
        float dy = circle_radius * sinf(theta);
        vertices.push_back(VertexFormat(circle_center + glm::vec3(dx, dy, 0), color));

        if (i > 0) {
            indices.push_back(center_index);
            indices.push_back(center_index + i);
            indices.push_back(center_index + i + 1);
        }
    }

    Mesh* tank_mesh = new Mesh(name);
    tank_mesh->InitFromData(vertices, indices);

    return tank_mesh;
}


Mesh* objects::CreateCircle(glm::vec3 center, float radius, int segments, glm::vec3 color, glm::vec3 center_color) {
    vector<VertexFormat> vertices3;
    vertices3.push_back(VertexFormat(center, center_color));

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * i / segments;
        float x = center.x + radius * cos(theta);
        float y = center.y + radius * sin(theta);
        vertices3.push_back(VertexFormat(glm::vec3(x, y, 0), color));
    }

    Mesh* circle = new Mesh("circle");
    vector<unsigned int> indices3;
    for (int i = 1; i <= segments; ++i) {
        indices3.push_back(0);
        indices3.push_back(i);
        indices3.push_back(i + 1);
    }
    indices3.push_back(0);
    indices3.push_back(segments);
    indices3.push_back(1);

    circle->InitFromData(vertices3, indices3);

    return circle;
}

void objects::UpdateCircle(Mesh* circle, glm::vec3 center, float radius, int segments, glm::vec3 color) {
    vector<VertexFormat> vertices3;
    vertices3.push_back(VertexFormat(center, color));

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * i / segments;
        float x = center.x + radius * cos(theta);
        float y = center.y + radius * sin(theta);
        vertices3.push_back(VertexFormat(glm::vec3(x, y, 0), color));
    }

    vector<unsigned int> indices3;
    for (int i = 1; i <= segments; ++i) {
        indices3.push_back(0);
        indices3.push_back(i);
        indices3.push_back(i + 1);
    }
    indices3.push_back(0);
    indices3.push_back(segments);
    indices3.push_back(1);

    circle->InitFromData(vertices3, indices3);
}

Mesh* objects::CreateRectangle(glm::vec3 middleBottom, float length, float width, glm::vec3 color) {
    glm::vec3 corner = middleBottom;
	corner.y -= width / 2.0f - 2;

	vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, width, 0), color),
		VertexFormat(corner + glm::vec3(0, width, 0), color)
	};
    
	Mesh* rectangle = new Mesh("rectangle");
	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

void objects::UpdateRectangle(Mesh* rectangle, glm::vec3 middleBottom, float length, float width, glm::vec3 color) {
    glm::vec3 corner = middleBottom;
    corner.y -= width / 2.0f - 2;

    vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    rectangle->InitFromData(vertices, indices);
}

Mesh* objects::CreateRectangleOutline(const glm::vec3& corner, float length, float height, const glm::vec3& color) {
    vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    vector<unsigned int> indices = {
        0, 1, 1, 2, 2, 3, 3, 0
    };

    Mesh* rectangleOutline = new Mesh("rectangleOutline");
    rectangleOutline->SetDrawMode(GL_LINES);
    rectangleOutline->InitFromData(vertices, indices);
    return rectangleOutline;
}

void objects::UpdateRectangleOutline(Mesh* rectangleOutline, const glm::vec3& corner, float length, float height, const glm::vec3& color) {
    vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    vector<unsigned int> indices = {
        0, 1, 1, 2, 2, 3, 3, 0
    };

    rectangleOutline->InitFromData(vertices, indices);
    rectangleOutline->SetDrawMode(GL_LINES);
}