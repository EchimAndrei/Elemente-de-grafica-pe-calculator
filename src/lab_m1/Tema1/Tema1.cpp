#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/objects.h"
#include <random>
#include <cmath>

using namespace std;
using namespace m1;

Tema1::Tema1()
{
    nr_of_segments = 1280;
    height_map = vector<glm::vec2>(1281);
    blue_translateX = 0;
    blue_translateY = 0;
    blue_rectangle_angle = 0;
    resolution = window->GetResolution();
    segment_size = resolution.x / nr_of_segments;
    blue_tank_length = 50;
    blue_rectangle_length = 50;
    blue_tank.projectile.magnitude = 300;
    blue_tank.alive = true;

    elapsed_time = 0;
    time_to_perform = 0;
    projectile_add_earth = 0;
    shake_intensity = 10.f;
    shake_duration = 0.2f;
    shake_speed = 0.5f;
    terrainMesh = new Mesh("terrain");
    trajectoryLine = new Mesh("trajectoryLine");

    height_threshold = 0.8f;
    height_transfer_epsilon = 50.f;

    red_translateX = 0;
    red_translateY = 0;
    red_tank_length = 50;
    red_rectangle_length = 50;
    red_rectangle_angle = 2.5;
    red_tank.projectile.magnitude = 300;
    red_tank.alive = true;

    explosion_duration = 1.f;
    explosion_time = 0.0f;

    max_particles = 50;
    particle_lifetime = 0.4f;
    particle_speed = 100.0f;
    particles.resize(max_particles);
    particle_mesh = objects::CreateCircle(glm::vec3(0, 0, 0), 5, 50, glm::vec3(183.f / 255.f, 169.f / 255.f, 37.f / 255.f), glm::vec3(1, 1, 1));

    bg_color = glm::vec3(146.0f / 255.0f, 197.0f / 255.0f, 233.0f / 255.0f);
    night_level = false;

    blue_tank_hitbox = objects::CreateCircle(glm::vec3(0, 0, 0), 30, 50, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
}

Tema1::~Tema1()
{
}

float terrain_function(float x) {
    float base_height = 300.f;
    float hill_height = 100.f;
    float hill_center = 640.f;
    float hill_width = 200.f;

    float terrain = 50.f * sin(0.015 * x) + 10 * sin(0.007 * x - 2) + 0.8 * sin(0.01 * x) + base_height;

    float distance_from_center = x - hill_center;
    float hill = hill_height * exp(-0.5 * (distance_from_center * distance_from_center) / (hill_width * hill_width));

    return terrain + hill;
}

vector<Cloud> Tema1::CreateClouds(float nr_of_clouds, vector<glm::vec2> cloud_positions, glm::vec3 color) {
    vector<Cloud> clouds;
    for (int i = 0; i < nr_of_clouds; i++) {
        float radius1 = 30.0f;
        float radius2 = 30.0f;

        glm::vec2 position = cloud_positions[i];

        Mesh* circle1 = objects::CreateCircle(glm::vec3(0, position.y, 0), radius1, 50, color, color);
        clouds.push_back({ position.x, position.y, circle1 });

        glm::vec2 offset = glm::vec2(radius1 / 1.5f, 0);
        Mesh* circle2 = objects::CreateCircle(glm::vec3(0 + offset.x, position.y, 0), radius2, 50, color, color);
        clouds.push_back({ position.x + offset.x, position.y, circle2 });

        glm::vec2 offset2 = glm::vec2(radius1 / 1.5f, 0);
        Mesh* circle3 = objects::CreateCircle(glm::vec3(0 + offset.x + offset2.x, position.y, 0), radius2, 50, color, color);
        clouds.push_back({ position.x + offset.x + offset2.x, position.y, circle3 });
    }
    return clouds;
}

vector<Mesh*> Tema1::CreateTerrain(float nr_of_segments, vector<glm::vec2> height_map, glm::vec3 color) {
    vector<Mesh*> triangles;
    vector<VertexFormat> allVertices;
    vector<unsigned int> allIndices;

    int indexOffset = 0;

    for (int i = 0; i < nr_of_segments; i++) {

        VertexFormat vertex1 = VertexFormat(glm::vec3(height_map[i].x, height_map[i].y, 0), color);
        VertexFormat vertex2 = VertexFormat(glm::vec3(height_map[i].x, 0, 0), color);
        VertexFormat vertex3 = VertexFormat(glm::vec3(height_map[i + 1].x, height_map[i + 1].y, 0), color);

        allVertices.push_back(vertex1);
        allVertices.push_back(vertex2);
        allVertices.push_back(vertex3);

        allIndices.push_back(indexOffset);
        allIndices.push_back(indexOffset + 1);
        allIndices.push_back(indexOffset + 2);
        indexOffset += 3;

        vertex1 = VertexFormat(glm::vec3(height_map[i + 1].x, height_map[i + 1].y, 0), color);
        vertex2 = VertexFormat(glm::vec3(height_map[i].x, 0, 0), color);
        vertex3 = VertexFormat(glm::vec3(height_map[i + 1].x, 0, 0), color);

        allVertices.push_back(vertex1);
        allVertices.push_back(vertex2);
        allVertices.push_back(vertex3);

        allIndices.push_back(indexOffset);
        allIndices.push_back(indexOffset + 1);
        allIndices.push_back(indexOffset + 2);
        indexOffset += 3;

    }

    terrainMesh->InitFromData(allVertices, allIndices);
    triangles.push_back(terrainMesh);

    return triangles;
}

void Tema1::Init()
{
    cloud_positions = { {200, 600}, {400, 550}, {600, 620} ,{800, 540}, {1000, 640} };
    clouds = CreateClouds(5, cloud_positions, glm::vec3(1, 1, 1));

    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    float segment_x = resolution.x / nr_of_segments;
    for (int i = 0; i <= nr_of_segments; i++) {
        height_map[i] = glm::vec2(segment_x * i, terrain_function(segment_x * i));
    }

    terrain = CreateTerrain(nr_of_segments, height_map, glm::vec3(183.f / 255.f, 169.f / 255.f, 37.f / 255.f));

    // ---------------- Spawn HealthPack ----------------
    srand((int)time(0));
    float healthPack_number = rand() % 5 + 1;
    float Pack_x, Pack_y;
    string pack_type;
    Mesh* mesh;
    for (int i = 0; i < healthPack_number; i++) {
        pack_type = "HealthPack";
        Pack_x = (float)(rand() % window->GetResolution().x - 20);
        Pack_x += 10;
        Pack_y = height_map[(int)(Pack_x / segment_size)].y + 10;
        mesh = objects::CreateCircle(glm::vec3(Pack_x, Pack_y, 0), 5, 50, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
        healthPacks.emplace_back(Pack_x, Pack_y, pack_type, mesh);
    }

    // ---------------- Spawn SpeedPack --------------
    float speedPack_number = rand() % 5 + 1;
    for (int i = 0; i < speedPack_number; i++) {
        pack_type = "HealthPack";
        Pack_x = (float)(rand() % window->GetResolution().x - 20);
        Pack_x += 10;
        Pack_y = height_map[(int)(Pack_x / segment_size)].y + 10;
        mesh = objects::CreateCircle(glm::vec3(Pack_x, Pack_y, 0), 5, 50, glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));
        speedPacks.emplace_back(Pack_x, Pack_y, pack_type, mesh);
    }

    // ---------------- Blue Tank -----------------------
    blue_tank.x = 0;
    blue_tank.y = 0;
    blue_tank.life = 100;
    blue_tank.mesh = objects::CreateTank(
        "blue_tank",
        glm::vec3(blue_tank.x, blue_tank.y, 0),
        blue_tank_length,
        glm::vec3(204.f / 255.f, 175.f / 255.f, 136.f / 255.f)
    );

    glm::vec3 center = glm::vec3(blue_tank.x, blue_tank.y, 0) + glm::vec3(0, blue_tank_length / 4.0f + blue_tank_length / 3.0f, 0);
    blue_rectangle = objects::CreateRectangle(center, blue_rectangle_length, 8, glm::vec3(0, 0, 0));
    blue_tank_healthBarOutline = objects::CreateRectangleOutline(center, blue_tank_length, 8, glm::vec3(0, 0, 0));
    blue_tank_healthBar = objects::CreateRectangle(glm::vec3(-blue_tank_length, blue_tank_length * 1.5f, 0), blue_tank.life, 10, glm::vec3(0, 1, 0));
    blue_circle = objects::CreateCircle(center, 5, 50, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));

    // ---------------- Red Tank ---------------------------
    red_tank.x = 1200;
    red_tank.y = height_map[(int)red_tank.x / segment_size].y;
    red_tank.life = 100;
    red_tank.mesh = objects::CreateTank(
        "red_tank",
        glm::vec3(0, 0, 0),
        red_tank_length,
        glm::vec3(255.f / 255.f, 155.f / 255.f, 155.f / 255.f)
    );
    glm::vec3 center_red = glm::vec3(0, red_tank_length / 4.0f + red_tank_length / 3.0f, 0);
    red_rectangle = objects::CreateRectangle(glm::vec3(0, red_tank_length / 4.0f + red_tank_length / 3.0f, 0), red_rectangle_length, 8, glm::vec3(0, 0, 0));
    red_tank_healthBarOutline = objects::CreateRectangleOutline(center_red, red_tank_length, 8, glm::vec3(0, 0, 0));
    red_tank_healthBar = objects::CreateRectangle(glm::vec3(-red_tank_length, red_tank_length * 1.5f, 0), red_tank.life, 10, glm::vec3(0, 1, 0));

    explosion_mesh = objects::CreateCircle(glm::vec3(0, 0, 0), 80, 50, glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
}

void Tema1::FrameStart()
{
    glClearColor(146.0f / 255.0f, 197.0f / 255.0f, 233.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

float Tema1::calculateTankRotation(int segment_index, const vector<glm::vec2>& height_map) {
    glm::vec2 A = height_map[segment_index];
    glm::vec2 B = height_map[segment_index + 1];
    glm::vec2 V = B - A;
    return atan2(V.y, V.x);
}

bool CheckCollision(glm::vec2 center1, float radius1, glm::vec2 center2, float radius2) {
    float distance = glm::distance(center1, center2);
    return distance <= radius1 + radius2;
}

void Tema1::RenderProjectileTrajectory(float startX, float startY, float initialSpeed, float angle, float g, glm::mat3 modelMatrix, glm::vec3 color) {
    float timeStep = 0.01f;
    float t = 0.0f;

    float v0x = initialSpeed * cos(angle);
    float v0y = initialSpeed * sin(angle);

    vector<glm::vec2> trajectoryPoints;

    float totalTime = (v0y + sqrt(v0y * v0y + 2 * g * startY)) / g;

    while (t <= totalTime) {
        float x = startX + v0x * t;
        float y = startY + v0y * t - 0.5f * g * t * t;

        if (x > 0 && x < resolution.x - 0 && y > 0) {
            int segment_index = x / segment_size;
            if (segment_index >= 0 && segment_index < height_map.size() && x < (height_map.size() * segment_size)) {
                if (y >= height_map[segment_index].y) {
                    trajectoryPoints.push_back(glm::vec2(x, y));
                }
                else {
                    break;
                }
            }
        }

        t += timeStep;
    }

    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    if (trajectoryPoints.empty()) return;
    vertices.clear();
    indices.clear();
    for (size_t i = 0; i < trajectoryPoints.size(); i++) {
        VertexFormat vertex = VertexFormat(glm::vec3(trajectoryPoints[i], 0), color);
        vertices.push_back(vertex);
        if (i > 0) {
            indices.push_back(i - 1);
            indices.push_back(i);
        }
    }

    if (!vertices.empty() && !indices.empty()) {
        trajectoryLine->SetDrawMode(GL_LINES);
        trajectoryLine->InitFromData(vertices, indices);

        glLineWidth(5.0f);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(0, 0);
        this->RenderMesh2D(trajectoryLine, shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);
    }
}

bool Tema1::CheckCollision_terrain(Projectile projectile) {
    int segment_index = projectile.x / segment_size;
    glm::vec2 P1 = height_map[segment_index];
    glm::vec2 P2 = height_map[segment_index + 1];

    float t = (projectile.x - P1.x) / (P2.x - P1.x);

    glm::vec2 I = P1 * (1 - t) + P2 * t;

    if (projectile.y - I.y <= 10) return true;
    else return false;
}

void Tema1::DeformTerrain(float explosionX, float explosionY, float radius) {
    int start = (int)(explosionX - radius);
    int end = (int)(explosionX + radius);

    start = max(0, start);
    end = min((int)nr_of_segments, end);

    for (int i = start; i <= end; i++) {
        float distance = glm::distance(glm::vec2(explosionX, explosionY), height_map[i]);
        if (distance < radius) {
            float loweringFactor = sqrt(radius * radius - distance * distance);
            height_map[i].y -= loweringFactor;
        }
    }
}

void Tema1::AddTerrain(float explosionX, float explosionY, float radius) {
    int start = (int)(explosionX - radius);
    int end = (int)(explosionX + radius);

    start = max(0, start);
    end = min((int)nr_of_segments, end);

    for (int i = start; i <= end; i++) {
        float distance = glm::distance(glm::vec2(explosionX, explosionY), height_map[i]);
        if (distance < radius) {
            height_map[i].y += (radius - distance);
        }
    }
}

float gaussian(float x, float sigma) {
    return exp(-0.5 * (x * x) / (sigma * sigma)) / (sigma * sqrt(2 * M_PI));
}

void Tema1::PerformHeightTransfer(float deltaTimeSeconds) {
    vector<float> smoothed_heights(nr_of_segments, 0.0f);
    float sigma = 0.6f;
    for (int i = 0; i < nr_of_segments; ++i) {
        float sum_weights = 0.0f;
        for (int j = -2; j <= 2; ++j) {
            int index = i + j;
            if (index >= 0 && index < nr_of_segments) {
                float weight = gaussian(j, sigma);
                smoothed_heights[i] += height_map[index].y * weight;
                sum_weights += weight;
            }
        }
        if (sum_weights > 0.0f) {
            smoothed_heights[i] /= sum_weights;
        }
    }

    for (int i = 0; i < nr_of_segments; ++i) {
        height_map[i].y = smoothed_heights[i];
    }
}


float GenerateRandomAngle(float radians, bool right, bool blue) {
    float random_value = (float)(rand()) / (float)(RAND_MAX);
    radians = -radians;
    float min_angle, max_angle;
    if (right) {
        radians = -radians;
        min_angle = glm::radians(160.0f) - radians;
        max_angle = glm::radians(260.0f) - radians;
        return min_angle + random_value * (max_angle - min_angle);
    }
    else if (!right) {
        min_angle = -(M_PI - glm::radians(160.0f)) + radians;
        max_angle = glm::radians(260.0f) - M_PI + radians;
        return min_angle + random_value * (max_angle - min_angle);
    }
}

float GenerateRandomSpeed() {
    float min_speed = 70.0f;
    float max_speed = 130.0f;
    float random_value = (float)(rand()) / (float)(RAND_MAX);
    return min_speed + random_value * (max_speed - min_speed);
}

void Tema1::Update(float deltaTimeSeconds) {

    if (night_level)
        bg_color = glm::vec3(100.0f / 255.0f, 140.0f / 255.0f, 180.0f / 255.0f);
    glClearColor(bg_color.x, bg_color.y, bg_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if ((!blue_tank.alive || !red_tank.alive) && !night_level) {
        float segment_x = resolution.x / nr_of_segments;
        for (int i = 0; i <= nr_of_segments; i++) {
            height_map[i] = glm::vec2(segment_x * i, terrain_function(segment_x * i));
        }
        night_level = true;
        blue_tank.life = 100;
        blue_tank.alive = true;
        blue_tank.x = 0;
        blue_tank.y = 0;
        red_tank.life = 100;
        red_tank.alive = true;
        red_tank.x = 1200;
        red_tank.y = height_map[(int)red_tank.x / segment_size].y;
        projectiles.clear();
    }

    for (auto& particle : particles) {
        if (particle.active) {
            particle.lifetime -= deltaTimeSeconds;
            if (particle.lifetime <= 0) {
                particle.active = false;
            }
            else {
                particle.position += particle.velocity * deltaTimeSeconds;
                blue_segment_index = blue_tank.x / segment_size;
                blue_tank.y = height_map[blue_segment_index].y;
                float blue_rotationAngle = calculateTankRotation(blue_segment_index, height_map);
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(particle.position.x, particle.position.y);
                this->RenderMesh2D(particle_mesh, shaders["VertexColor"], modelMatrix);
            }
        }
    }

    // ---------------- Camera Shake ---------------
    if (shake_time > 0.0f) {
        float offsetX = ((rand() % 100) / 100.0f - 0.5f) * 2 * shake_intensity;
        float offsetY = ((rand() % 100) / 100.0f - 0.5f) * 2 * shake_intensity;

        auto camera = GetSceneCamera();
        camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
        camera->SetPosition(glm::vec3(0, 0, 50));
        camera->SetRotation(glm::vec3(0, 0, 0));
        camera->Update();
        camera->SetPosition(glm::vec3(offsetX, offsetY, 50));
        camera->Update();

        shake_time -= deltaTimeSeconds;

        if (shake_time <= 0.0f) {
            camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
            camera->SetPosition(glm::vec3(0, 0, 50));
            camera->SetRotation(glm::vec3(0, 0, 0));
            camera->Update();
        }
    }

    // ---------------- Health Pack ---------------
    for (auto& healthPack : healthPacks) {
        if (healthPack.x < 10 || healthPack.x > resolution.x - 10 || healthPack.y < 10 || healthPack.y > resolution.y - 10) {
            healthPack.x = (float)(rand() % window->GetResolution().x);
            healthPack.y = height_map[(int)(healthPack.x / segment_size)].y + 5;
        }
        if (healthPack.active)
            this->RenderMesh2D(healthPack.mesh, shaders["VertexColor"], glm::mat3(1));

        if (abs(blue_tank.x - healthPack.x) <= 20 && abs(blue_tank.y - healthPack.y) <= 20 && healthPack.active) {
            if (blue_tank.life != 100) {
                blue_tank.life += 10;
                healthPack.active = false;
            }
        }
        if (abs(red_tank.x - healthPack.x) <= 20 && abs(red_tank.y - healthPack.y) <= 2 && healthPack.active) {
            if (red_tank.life != 100) {
                red_tank.life += 10;
                healthPack.active = false;
            }
        }
    }

    // ----------------- Speed Pack ----------------
    for (auto& speedPack : speedPacks) {
        if (speedPack.x < 10 || speedPack.x > resolution.x - 10 || speedPack.y < 10 || speedPack.y > resolution.y - 10) {
            speedPack.x = (float)(rand() % window->GetResolution().x);
            speedPack.y = height_map[(int)(speedPack.x / segment_size)].y + 5;
        }

        if (speedPack.active)
            this->RenderMesh2D(speedPack.mesh, shaders["VertexColor"], glm::mat3(1));

        if (abs(blue_tank.x - speedPack.x) <= 20 && abs(blue_tank.y - speedPack.y) <= 20 && speedPack.active) {
            blue_tank.speed += 10.f;
            cout << blue_tank.speed << endl;
            speedPack.active = false;
        }
        if (abs(red_tank.x - speedPack.x) <= 20 && abs(red_tank.y - speedPack.y) <= 20 && speedPack.active) {
            red_tank.speed += 10.f;
            speedPack.active = false;
        }
    }


    // ---------------- Verificari -----------------
    blue_segment_index = blue_tank.x / segment_size;
    blue_tank.y = height_map[blue_segment_index].y;
    float blue_rotationAngle = calculateTankRotation(blue_segment_index, height_map);

    if (blue_tank.x + blue_tank_length > resolution.x) {
        blue_tank.x = resolution.x - blue_tank_length;
    }
    if (blue_tank.x - blue_tank_length < 0) {
        blue_tank.x = blue_tank_length;
    }
    if (blue_rectangle_angle - blue_rotationAngle > M_PI) {
        blue_rectangle_angle = M_PI + blue_rotationAngle;
    }
    if (blue_rectangle_angle - blue_rotationAngle < 0) {
        blue_rectangle_angle = blue_rotationAngle;
    }
    // ------------------ Renderizare ------------------

    // ---------------- Blue Tank -----------------------
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(blue_tank.x, blue_tank.y);
    modelMatrix *= transform2D::Rotate(blue_rotationAngle);
    if (blue_tank.alive) {
        this->RenderMesh2D(blue_tank.mesh, shaders["VertexColor"], modelMatrix);


        // ---------------- Health Bar ----------------
        objects::UpdateRectangleOutline(blue_tank_healthBarOutline, glm::vec3(-blue_tank_length, blue_tank_length * 1.5f - 2.5f, 0), blue_tank_length + blue_tank_length, 10, glm::vec3(0, 0, 0));
        this->RenderMesh2D(blue_tank_healthBarOutline, shaders["VertexColor"], modelMatrix);


        // ---------------- Health Bar ----------------
        objects::UpdateRectangle(blue_tank_healthBar, glm::vec3(-blue_tank_length, blue_tank_length * 1.5f, 0), blue_tank.life, 10, glm::vec3(0, 1, 0));
        this->RenderMesh2D(blue_tank_healthBar, shaders["VertexColor"], modelMatrix);
    }

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(blue_tank.x, blue_tank.y);
    modelMatrix *= transform2D::Rotate(blue_rotationAngle);
    modelMatrix *= transform2D::Translate(0, blue_tank_length / 4.0f + blue_tank_length / 3.0f + 2.0f);
    modelMatrix *= transform2D::Rotate(blue_rectangle_angle - blue_rotationAngle);
    modelMatrix *= transform2D::Translate(0, -(blue_tank_length / 4.0f + blue_tank_length / 3.0f) - 2.0f);
    if (blue_tank.alive)
        this->RenderMesh2D(blue_rectangle, shaders["VertexColor"], modelMatrix);

    // ---------------- Red Tank -----------------------
    modelMatrix = glm::mat3(1);
    int red_segment_index = red_tank.x / segment_size;
    red_tank.y = height_map[red_segment_index].y;
    modelMatrix *= transform2D::Translate(red_tank.x, red_tank.y);
    float red_rotationAngle = calculateTankRotation(red_segment_index, height_map);
    modelMatrix *= transform2D::Rotate(red_rotationAngle);
    if (red_tank.x + red_tank_length > resolution.x) {
        red_tank.x = resolution.x - red_tank_length;
    }
    if (red_tank.x - red_tank_length < 0) {
        red_tank.x = red_tank_length;
    }
    if (red_rectangle_angle - red_rotationAngle > M_PI) {
        red_rectangle_angle = M_PI + red_rotationAngle;
    }
    if (red_rectangle_angle - red_rotationAngle < 0) {
        red_rectangle_angle = red_rotationAngle;
    }

    if (red_tank.alive) {
        this->RenderMesh2D(red_tank.mesh, shaders["VertexColor"], modelMatrix);

        // ---------------- Health Bar ----------------
        objects::UpdateRectangleOutline(red_tank_healthBarOutline, glm::vec3(-red_tank_length, red_tank_length * 1.5f - 2.5f, 0), red_tank_length + red_tank_length, 10, glm::vec3(0, 0, 0));
        this->RenderMesh2D(red_tank_healthBarOutline, shaders["VertexColor"], modelMatrix);

        // ---------------- Health Bar ----------------
        objects::UpdateRectangle(red_tank_healthBar, glm::vec3(-red_tank_length, red_tank_length * 1.5f, 0), red_tank.life, 10, glm::vec3(0, 1, 0));
        this->RenderMesh2D(red_tank_healthBar, shaders["VertexColor"], modelMatrix);
    }

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(red_tank.x, red_tank.y);
    modelMatrix *= transform2D::Rotate(red_rotationAngle);
    modelMatrix *= transform2D::Translate(0, red_tank_length / 4.0f + red_tank_length / 3.0f + 2.0f);
    modelMatrix *= transform2D::Rotate(red_rectangle_angle - red_rotationAngle);
    modelMatrix *= transform2D::Translate(0, -(red_tank_length / 4.0f + red_tank_length / 3.0f) - 2.0f);
    if (red_tank.alive)
        this->RenderMesh2D(red_rectangle, shaders["VertexColor"], modelMatrix);

    // ------------------ Proiectile ------------------
    for (auto& projectile : projectiles) {
        if (!projectile.valid)
            continue;
        if (projectile.x < 10 || projectile.x > resolution.x - 10) {
            projectile.valid = false;
            continue;
        }
        if (projectile_add_earth)
            projectile.type = 1;
        projectile.x += projectile.speed.x * deltaTimeSeconds;
        projectile.y += projectile.speed.y * deltaTimeSeconds;
        projectile.speed.y -= 9.8f * 20.f * deltaTimeSeconds;
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(projectile.x, projectile.y);
        modelMatrix *= transform2D::Rotate(projectile.angle);
        this->RenderMesh2D(projectile.mesh, shaders["VertexColor"], modelMatrix);

        float projectile_radius = 5;
        float blue_tank_radius = 30;
        glm::vec2 projectile_center = glm::vec2(projectile.x, projectile.y);
        glm::vec2 blue_tank_center = glm::vec2(blue_tank.x, height_map[(int)(blue_tank.x / segment_size)].y);
        if (CheckCollision(projectile_center, projectile_radius, blue_tank_center, blue_tank_radius)) {
            blue_tank.life -= 10;
            if (blue_tank.life <= 0) {
                blue_tank.alive = false;
            }
            if (blue_tank.alive == true) {
                explosion_time += deltaTimeSeconds;
                if (explosion_time <= explosion_duration) {
                    glm::mat3 modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(projectile.x, projectile.y);
                    this->RenderMesh2D(explosion_mesh, shaders["VertexColor"], modelMatrix);
                }
            }
            projectile.valid = false;
        }
        glm::vec2 red_tank_center = glm::vec2(red_tank.x, height_map[(int)(red_tank.x / segment_size)].y);
        if (CheckCollision(projectile_center, projectile_radius, red_tank_center, blue_tank_radius)) {
            red_tank.life -= 10;
            if (red_tank.life <= 0) {
                red_tank.alive = false;
            }
            if (red_tank.alive == true)
            {
                explosion_time += deltaTimeSeconds;
                if (explosion_time <= explosion_duration) {
                    glm::mat3 modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(projectile.x, projectile.y);
                    this->RenderMesh2D(explosion_mesh, shaders["VertexColor"], modelMatrix);
                }
            }
            projectile.valid = false;
        }
        if (CheckCollision_terrain(projectile) && projectile.valid) {
            float explosion_radius = 100;
            if (!projectile_add_earth)
                DeformTerrain(projectile.x, projectile.y, explosion_radius);
            else
                AddTerrain(projectile.x, projectile.y, explosion_radius);
            explosion_time += deltaTimeSeconds;
            if (explosion_time <= explosion_duration) {
                glm::mat3 modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectile.x, projectile.y);
                this->RenderMesh2D(explosion_mesh, shaders["VertexColor"], modelMatrix);
            }
            projectile.valid = false;
        }
    }

    // ------------------ Traiectorie ------------------
    if (blue_tank.alive) {
        float blue_tank_rotation = calculateTankRotation(blue_tank.x, height_map);
        float base_rectangle_x = (blue_tank_length / 4.0f + blue_tank_length / 3.0f + 2.0f) * cos(M_PI / 2.0f + blue_tank_rotation);
        float base_rectangle_y = (blue_tank_length / 4.0f + blue_tank_length / 3.0f + 2.0f) * sin(M_PI / 2.0f + blue_tank_rotation);
        glm::vec3 cannon_base = glm::vec3(blue_tank.x, blue_tank.y, 0) +
            glm::vec3(base_rectangle_x, base_rectangle_y, 0);
        float total_angle = blue_rectangle_angle;
        glm::vec3 projectile_offset;
        projectile_offset.x = cos(total_angle) * blue_rectangle_length;
        projectile_offset.y = sin(total_angle) * blue_rectangle_length;
        glm::vec3 projectile_center = cannon_base + projectile_offset;
        float g = 9.8f * 20.f;
        this->RenderProjectileTrajectory(projectile_center.x, projectile_center.y, blue_tank.projectile.magnitude, total_angle, g, modelMatrix, glm::vec3(0, 0, 1));
    }
    if (red_tank.alive) {
        float red_tank_rotation = calculateTankRotation((int)(red_tank.x / segment_size), height_map);
        float base_rectangle_x = (red_tank_length / 4.0f + red_tank_length / 3.0f + 2.0f) * cos(M_PI / 2.0f + red_tank_rotation);
        float base_rectangle_y = (red_tank_length / 4.0f + red_tank_length / 3.0f + 2.0f) * sin(M_PI / 2.0f + red_tank_rotation);
        glm::vec3 cannon_base = glm::vec3(red_tank.x, red_tank.y, 0) +
            glm::vec3(base_rectangle_x, base_rectangle_y, 0);
        float total_angle = red_rectangle_angle;
        glm::vec3 projectile_offset;
        projectile_offset.x = cos(total_angle) * red_rectangle_length;
        projectile_offset.y = sin(total_angle) * red_rectangle_length;
        glm::vec3 projectile_center = cannon_base + projectile_offset;
        float g = 9.8f * 20.f;
        this->RenderProjectileTrajectory(projectile_center.x, projectile_center.y, red_tank.projectile.magnitude, total_angle, g, modelMatrix, glm::vec3(1, 0, 0));
    }



    for (auto& cloud : clouds) {
        cloud.x += 20 * deltaTimeSeconds;
        if (cloud.x >= resolution.x) {
            cloud.x -= 1400.f;
        }
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(cloud.x, 0);
        this->RenderMesh2D(cloud.mesh, shaders["VertexColor"], modelMatrix);
    }

    // ------------------ Teren ------------------
    PerformHeightTransfer(deltaTimeSeconds);
    if (!night_level)
        terrain = CreateTerrain(nr_of_segments, height_map, glm::vec3(183.f / 255.f, 169.f / 255.f, 37.f / 255.f));
    else
    {
        terrain = CreateTerrain(nr_of_segments, height_map, glm::vec3(130.f / 255.f, 120.f / 255.f, 1.f / 255.f));
    }

    modelMatrix = glm::mat3(1);
    for (auto triangle : terrain) {
        this->RenderMesh2D(triangle, shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::FrameEnd()
{
}

void Tema1::GenerateParticle(Tank& tank, float tank_length, float tank_angle, bool blue, bool right) {
    if (blue)
        tank.y = height_map[(int)(blue_tank.x / segment_size)].y;
    else
        tank.y = height_map[(int)(red_tank.x / segment_size)].y;
    for (auto& particle : particles) {
        if (!particle.active) {
            particle.active = true;
            particle.position = glm::vec2(
                tank.x + cos(M_PI / 2 + tank_angle),
                tank.y + sin(M_PI / 2 + tank_angle)
            );
            float angle = GenerateRandomAngle(tank_angle, right, blue);
            float speed = GenerateRandomSpeed();
            particle.velocity = glm::vec2(speed * cos(angle), speed * sin(angle));
            particle.lifetime = particle_lifetime;
            break;
        }
    }
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    float angular_speed = 1.5f;

    if (window->KeyHold(GLFW_KEY_A) && blue_tank.alive) {
        float blue_tank_rotation = calculateTankRotation(((int)blue_tank.x / segment_size), height_map);

        if (blue_tank_rotation != 0 && height_map[(int)(blue_tank.x / segment_size)].y > height_map[(int)(blue_tank.x / segment_size) + 1].y) {
            blue_tank_state = 1;
        }
        else if (blue_tank_rotation != 0 && height_map[(int)(blue_tank.x / segment_size)].y < height_map[(int)(blue_tank.x / segment_size) + 1].y) {
            blue_tank_state = 2;
        }
        if (blue_tank_state == 0) {
            blue_tank.x -= blue_tank.speed * deltaTime;
        }
        else if (blue_tank_state == 1) {
            blue_tank.x -= blue_tank.speed * deltaTime / 1.3f * cos(blue_tank_rotation);
        }
        else {
            blue_tank.x -= blue_tank.speed * deltaTime * 1.5f * cos(blue_tank_rotation);
        }

        GenerateParticle(blue_tank, blue_tank_length, blue_tank_rotation, 1, 0);
    }

    if (window->KeyHold(GLFW_KEY_D) && blue_tank.alive) {
        float blue_tank_rotation = calculateTankRotation(((int)blue_tank.x / segment_size), height_map);

        if (blue_tank_rotation != 0 && height_map[(int)(blue_tank.x / segment_size)].y < height_map[(int)(blue_tank.x / segment_size) + 1].y) {
            blue_tank_state = 1;
        }
        else if (blue_tank_rotation != 0 && height_map[(int)(blue_tank.x / segment_size)].y > height_map[(int)(blue_tank.x / segment_size) + 1].y) {
            blue_tank_state = 2;
        }
        if (blue_tank_state == 0) {
            blue_tank.x += blue_tank.speed * deltaTime;
        }
        else if (blue_tank_state == 1) {
            blue_tank.x += blue_tank.speed * deltaTime / 1.3f * cos(blue_tank_rotation);
        }
        else {
            blue_tank.x += blue_tank.speed * deltaTime * 1.5f * cos(blue_tank_rotation);
        }
        GenerateParticle(blue_tank, blue_tank_length, blue_tank_rotation, 1, 1);
    }

    if (window->KeyHold(GLFW_KEY_W) && blue_tank.alive) {
        blue_rectangle_angle += angular_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S) && blue_tank.alive) {
        blue_rectangle_angle -= angular_speed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_LEFT) && red_tank.alive) {

        float red_tank_rotation = calculateTankRotation((int)(red_tank.x / segment_size), height_map);

        if (red_tank_rotation != 0 && height_map[(int)(red_tank.x / segment_size)].y < height_map[(int)(red_tank.x / segment_size) - 1].y) {
            red_tank_state = 1;
        }
        else if (red_tank_rotation != 0 && height_map[(int)(red_tank.x / segment_size)].y > height_map[(int)(red_tank.x / segment_size) - 1].y) {
            red_tank_state = 2;
        }
        if (red_tank_state == 0) {
            red_tank.x -= red_tank.speed * deltaTime;
        }
        else if (red_tank_state == 1) {
            red_tank.x -= red_tank.speed * deltaTime / 1.3f * cos(red_tank_rotation);
        }
        else {
            red_tank.x -= red_tank.speed * deltaTime * 1.5f * cos(red_tank_rotation);
        }
        red_tank.y = terrain_function(red_tank.x);
        GenerateParticle(red_tank, red_tank_length, red_tank_rotation, 0, 0);
    }

    if (window->KeyHold(GLFW_KEY_RIGHT) && red_tank.alive) {
        float red_tank_rotation = calculateTankRotation((int)(red_tank.x / segment_size), height_map);

        if (red_tank_rotation != 0 && height_map[(int)(red_tank.x / segment_size)].y < height_map[(int)(red_tank.x / segment_size) + 1].y) {
            red_tank_state = 1;
        }
        else if (red_tank_rotation != 0 && height_map[(int)(red_tank.x / segment_size)].y > height_map[(int)(red_tank.x / segment_size) + 1].y) {
            red_tank_state = 2;
        }
        if (red_tank_state == 0) {
            red_tank.x += red_tank.speed * deltaTime;
        }
        else if (red_tank_state == 1) {
            red_tank.x += red_tank.speed * deltaTime / 1.3f * cos(red_tank_rotation);
        }
        else {
            red_tank.x += red_tank.speed * deltaTime * 1.5f * cos(red_tank_rotation);
        }
        red_tank.y = terrain_function(red_tank.x);
        GenerateParticle(red_tank, red_tank_length, red_tank_rotation, 0, 1);
    }
    if (window->KeyHold(GLFW_KEY_UP) && red_tank.alive) {
        red_rectangle_angle -= angular_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_DOWN) && red_tank.alive) {
        red_rectangle_angle += angular_speed * deltaTime;
    }

}

void Tema1::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_SPACE && blue_tank.alive) {

        // ---------------- Camera Shake ---------------
        shake_time = shake_duration;
        // ---------------- Camera Shake ---------------


        float blue_tank_rotation = calculateTankRotation(blue_tank.x, height_map);
        float base_rectangle_x = (blue_tank_length / 4.0f + blue_tank_length / 3.0f + 2.0f) * cos(M_PI / 2.0f + blue_tank_rotation);
        float base_rectangle_y = (blue_tank_length / 4.0f + blue_tank_length / 3.0f + 2.0f) * sin(M_PI / 2.0f + blue_tank_rotation);
        glm::vec3 cannon_base = glm::vec3(blue_tank.x, blue_tank.y, 0) +
            glm::vec3(base_rectangle_x, base_rectangle_y, 0);

        glm::vec3 projectile_offset;
        projectile_offset.x = cos(blue_rectangle_angle) * blue_rectangle_length;
        projectile_offset.y = sin(blue_rectangle_angle) * blue_rectangle_length;

        glm::vec3 projectile_center = cannon_base + projectile_offset;
        if (!projectile_add_earth)
            projectile_mesh = objects::CreateCircle(glm::vec3(0, 0, 0), 2.5f, 50, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
        else
            projectile_mesh = objects::CreateTank("haha", glm::vec3(0, 0, 0), 10.5f, glm::vec3(1, 0, 1));
        blue_tank.projectile.mesh = projectile_mesh;

        glm::vec2 speed;
        speed.x = cos(blue_rectangle_angle) * blue_tank.projectile.magnitude;
        speed.y = sin(blue_rectangle_angle) * blue_tank.projectile.magnitude;
        blue_tank.projectile.angle = blue_rectangle_angle;
        blue_tank.projectile.speed = speed;
        blue_tank.projectile.radius = 5;

        projectiles.emplace_back(blue_tank.projectile.mesh, projectile_center.x, projectile_center.y,
            blue_tank.projectile.speed, blue_tank.projectile.magnitude, blue_rectangle_angle, blue_tank.projectile.radius);
    }

    if (key == GLFW_KEY_ENTER && red_tank.alive) {
        float red_tank_rotation = calculateTankRotation((int)(red_tank.x / segment_size), height_map);
        float base_rectangle_x = (red_tank_length / 4.0f + red_tank_length / 3.0f + 2.0f) * cos(M_PI / 2.0f + red_tank_rotation);
        float base_rectangle_y = (red_tank_length / 4.0f + red_tank_length / 3.0f + 2.0f) * sin(M_PI / 2.0f + red_tank_rotation);
        glm::vec3 cannon_base = glm::vec3(red_tank.x, red_tank.y, 0) +
            glm::vec3(base_rectangle_x, base_rectangle_y, 0);

        glm::vec3 projectile_offset;
        projectile_offset.x = cos(red_rectangle_angle) * red_rectangle_length;
        projectile_offset.y = sin(red_rectangle_angle) * red_rectangle_length;

        glm::vec3 projectile_center = cannon_base + projectile_offset;
        projectile_mesh = objects::CreateCircle(glm::vec3(0, 0, 0), 2.5f, 50, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
        if (!projectile_add_earth)
            projectile_mesh = objects::CreateCircle(glm::vec3(0, 0, 0), 2.5f, 50, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
        else
            projectile_mesh = objects::CreateTank("haha", glm::vec3(0, 0, 0), 10.5f, glm::vec3(1, 0, 1));
        red_tank.projectile.mesh = projectile_mesh;

        glm::vec2 speed;
        speed.x = cos(red_rectangle_angle) * red_tank.projectile.magnitude;
        speed.y = sin(red_rectangle_angle) * red_tank.projectile.magnitude;
        red_tank.projectile.angle = red_rectangle_angle;
        red_tank.projectile.speed = speed;
        red_tank.projectile.radius = 5;

        projectiles.emplace_back(red_tank.projectile.mesh, projectile_center.x, projectile_center.y,
            red_tank.projectile.speed, red_tank.projectile.magnitude, red_rectangle_angle, red_tank.projectile.radius);
    }

    if (key == GLFW_KEY_R) {
        blue_tank.life = 100;
        red_tank.life = 100;
    }
    if (key == GLFW_KEY_T) {
        if (projectile_add_earth)
            projectile_add_earth = 0;
        else
            projectile_add_earth = 1;
    }
    if (key == GLFW_KEY_E && blue_tank.alive) {
        blue_tank.projectile.magnitude += 50;
        cout << "speed: " << blue_tank.projectile.magnitude << endl;
    }
    if (key == GLFW_KEY_Q && blue_tank.alive) {
        blue_tank.projectile.magnitude -= 50;
        cout << "speed: " << blue_tank.projectile.magnitude << endl;
    }
}
