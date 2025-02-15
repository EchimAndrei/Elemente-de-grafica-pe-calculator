#pragma once

#include "components/simple_scene.h"
#include "camera2.h"

using namespace std;
using namespace implemented;

namespace m1
{

    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct Packet {
            glm::vec3 spawn;
			glm::vec3 destination;
			glm::vec3 position;
			float length;

            Mesh* packet_mesh;
            Mesh* spawn_mesh;
			Mesh* destination_mesh;

            float health = 100.f;

			Packet() {
				spawn = glm::vec3(0, 0, 0);
				destination = glm::vec3(0, 0, 0);
                position = glm::vec3(0, 0, 0);
			}
			Packet(glm::vec3 spawn, glm::vec3 destision, glm::vec3 position, float length, Mesh* packet_mesh, Mesh* spawn_mesh, Mesh* destination_mesh)
				: spawn(spawn), destination(destision), position(position), packet_mesh(packet_mesh), length(length),  spawn_mesh(spawn_mesh), destination_mesh(destination_mesh){}
        };

        struct Drone
        {
            Mesh* drone_mesh;
            glm::vec3 position;
            glm::vec3 previous_position;
            glm::vec3 movement;
            float length;

            float scaleX, scaleY, scaleZ;
            float angularStepOX, angularStepOY, angularStepOZ;

            float speed;
            glm::vec3 color;

            bool HasPacket = false;

            Drone()
            {
                position = glm::vec3(0, 0, 0);
                length = 1;

                scaleX = 1;
                scaleY = 1;
                scaleZ = 1;

                angularStepOX = 0;
                angularStepOY = 0;
                angularStepOZ = 0;

                speed = 1;
                color = glm::vec3(0, 0, 1);
            }
        };

        struct Tree {
            float x, y, z;
            Mesh* mesh;
            float width, height;

            glm::vec3 trunkMin;
            glm::vec3 trunkMax;

            glm::vec3 cone1Min;
            glm::vec3 cone1Max;

            glm::vec3 cone2Min;
            glm::vec3 cone2Max;

            Tree(float x, float y, float z, float width, float height, Mesh* mesh)
                : x(x), y(y), z(z), width(width), height(height), mesh(mesh) {}
        };

        struct House {
            float x, y, z;
            Mesh* mesh;
            float width, height;

            glm::vec3 baseMin;
            glm::vec3 baseMax;

            glm::vec3 roofMin;
            glm::vec3 roofMax;

            House(float x, float y, float z, float width, float height, Mesh* mesh)
                : x(x), y(y), z(z), width(width), height(height), mesh(mesh) {}
        };


        Tema2();
        ~Tema2();

        void Init() override;

        bool isInExcludedZone(float x, float z, float exclusionRadius);
        bool isTooCloseToExistingTree(float x, float z, float minDistance);
        bool isTooCloseToExistingHouse(float x, float z, float minDistance);

        m1::Tema2::House createHouse(float x, float y, float z, float width, float height);
        float GetTerrainHeightAtPosition(float x, float z);

    private:
        void FrameStart() override;
        bool Tema2::AABBvsAABB(glm::vec3 minA, glm::vec3 maxA, glm::vec3 minB, glm::vec3 maxB);
        void RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderPropeller(float x, float y, float z, const glm::mat4& droneTransform, Camera2* camera);
        glm::vec3 getRandomPosition();
        void RenderScene(Camera2* camera, Packet* packet);
        bool collision_with_tree(glm::vec3 droneMin, glm::vec3 droneMax, Tree tree);
        bool Tema2::collision_with_house(glm::vec3 droneMin, glm::vec3 droneMax, House house);
        void RenderTerrain(Drone drone, Camera2* camera);
        m1::Tema2::Tree createTree(float x, float y, float z, float width, float height);
        glm::mat4 Tema2::calculateArrowMatrix(Drone drone, Packet* packet);
        bool Tema2::pickUpPacket(Packet packet, Drone drone);
        bool Tema2::deliveredPacket(Packet* packet, Drone drone);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

    protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        GLenum polygonMode;
        ViewportArea miniViewportArea;

        float m_terrain_rows, n_terrain_columns;
        vector<VertexFormat> terrain_vertices;

        
        
        Drone properties;
        Drone drone;
        Mesh* drone_mesh;
        Mesh* elice_mesh;
        float elice_angularStep;

        // Camera properties
        implemented::Camera2* camera;
        implemented::Camera2* predicted_camera;
        glm::mat4 projectionMatrix;

        implemented::Camera2* ortho_camera;
		glm::mat4 ortho_projectionMatrix;

        float distanceToDrone = 2;

        float can_move = 1;

        // Terrain
        Mesh* terrain;
        vector<Tree> trees;
		vector<House> houses;

        // Packet
		Packet* packet;
        Mesh* packet_mesh;
		Mesh* spawn_checkpoint_mesh;
		Mesh* destination_checkpoint_mesh;
        glm::mat4 packetMatrix = glm::mat4(1);
        glm::mat4 spawnmatrix = glm::mat4(1);
        glm::mat4 destinationMatrix = glm::mat4(1);
        glm::mat4 arrowMatrix= glm::mat4(1);
		glm::mat4 spawnCheckpointMatrix = glm::mat4(1);
		glm::mat4 destinationCheckpointMatrix = glm::mat4(1);

		glm::vec3 last_hit_position = glm::vec3(0,0,0);

        Mesh* arrow_mesh;

        float score = 0;
    };
}
