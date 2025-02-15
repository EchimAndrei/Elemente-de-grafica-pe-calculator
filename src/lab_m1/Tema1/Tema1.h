#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"
#include <iostream>
#include <cmath>
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;

namespace m1
{
    struct Cloud {
        float x, y;
		Mesh* mesh;
    };
    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        float lifetime;
        bool active;

        Particle() : position(0), velocity(0), lifetime(0), active(false) {}
    };
    struct Pack {
        float x, y;
        string type;
        bool active;
		Mesh* mesh;
        Pack(float x, float y, string type, Mesh* mesh) : x(x), y(y), type(type), active(true), mesh(mesh) {};
    };

    struct Projectile {
        Mesh* mesh;
        float x;
        float y;
        glm::vec2 speed;
        float magnitude;
        float angle;
        bool valid;
        float radius;
        int type;

        Projectile() : mesh(nullptr), x(0), y(0), speed(2.0f, 0.0f), magnitude(0), angle(0), valid(true){}
        Projectile(Mesh* mesh, float x, float y, glm::vec2 speed, float magnitude, float angle, float radius)
            : mesh(mesh), x(x), y(y), speed(speed), magnitude(magnitude), angle(angle), valid(true), radius(radius), type(0) {}
    };

    struct Tank {
        float x;
        float y;
        float life;
        Mesh* mesh;
        bool alive;
        float speed;
        Projectile projectile;
        Tank() : x(0), y(0), life(0), mesh(), alive(true), projectile(), speed(75.f){}
    };


    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();


        vector<Cloud> CreateClouds(float nr_of_clouds, vector<glm::vec2> cloud_positions, glm::vec3 color);

        vector<Mesh*> CreateTerrain(float nr_of_segments, vector<glm::vec2> height_map, glm::vec3 color);


        void Init();

    private:
        void FrameStart();
        float calculateTankRotation(int segment_index, const std::vector<glm::vec2>& height_map);
        void RenderProjectileTrajectory(float startX, float startY, float initialSpeed, float angle, float g, glm::mat3 modelMatrix, glm::vec3 color);
        bool CheckCollision_terrain(Projectile projectile);
        void DeformTerrain(float explosionX, float explosionY, float radius);
        void AddTerrain(float explosionX, float explosionY, float radius);
        void PerformHeightTransfer(float deltaTimeSeconds);
        void Update(float deltaTimeSeconds);
        void FrameEnd();
        void GenerateParticle(Tank& tank, float tank_length, float tank_angle, bool blue, bool right);

        void OnInputUpdate(float deltaTime, int mods);
        void OnKeyPress(int key, int mods);

		float shake_intensity;
		float shake_duration;
		float shake_speed;
        float shake_time;

		glm::ivec2 resolution;
        float segment_size;
        float red_segment_index;
        float blue_segment_index;
        float nr_of_segments;
        vector<Pack> healthPacks;
        Mesh* healthpack_mesh;
        vector<Pack> speedPacks;
        Mesh* speedpack_mesh;

        std::vector<glm::vec2> height_map;
        vector<Mesh*> terrain;
        std::vector<glm::vec2> trajectory_map;
        vector<Mesh*> trajectory;
        glm::mat3 modelMatrix;
        Tank blue_tank;
        Mesh* blue_circle;
        Mesh* blue_rectangle;
        Mesh* blue_tank_healthBar;
		Mesh* blue_tank_healthBarOutline;
        float blue_rectangle_angle, blue_tank_length, blue_rectangle_length;
        float blue_translateX, blue_translateY;
        vector<Projectile> projectiles;
		float height_threshold, height_transfer_epsilon;
        bool recalculateTrajectory;
        std::vector<glm::vec2> trajectoryPoints;
		int elapsed_time;
        int time_to_perform;
        int projectile_add_earth;
        Mesh* projectile_mesh;
        Mesh* terrainMesh;
        Mesh* trajectoryLine;

        Tank red_tank;
        Mesh* red_circle;
        Mesh* red_rectangle;
        Mesh* red_tank_healthBar;
        Mesh* red_tank_healthBarOutline;
        float red_rectangle_angle, red_tank_length, red_rectangle_length;
        float red_translateX, red_translateY;
		Mesh* explosion_mesh;
        float explosion_duration;
        float explosion_time;

        // 0 - drept, 1 - urca, 2 - coboara
        int blue_tank_state;
		int red_tank_state;

        std::vector<Particle> particles;
        int max_particles;
        float particle_lifetime;
        float particle_speed;
        Mesh* particle_mesh;

        float night_level;
        glm::vec3 bg_color;

        Mesh* blue_tank_hitbox;
		Mesh* red_tank_hitbox;

        Mesh* cloudMesh;
        vector<glm::vec2> cloud_positions;
		vector<Cloud> clouds;
        
};
}