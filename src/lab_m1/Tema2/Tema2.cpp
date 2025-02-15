#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/Tema2/transform3D.h"
#include "lab_m1/Tema2/objects2.h"

using namespace std;
using namespace m1;

Tema2::Tema2() {
	elice_angularStep = 0;
	drone = Drone();
}

Tema2::~Tema2()
{
}

void Tema2::FrameStart()
{
    glClearColor(139.f / 225.f, 168.f / 225.f, 198.f / 225.f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema2::Init()
{
    polygonMode = GL_FILL;

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

    drone_mesh = objects2::CreateDrone(drone.position, drone.length);
    elice_mesh = objects2::CreateElice(drone.position, drone.length);

    //----------------------- CAMERA -----------------------
    camera = new Camera2();
    predicted_camera = new Camera2(*camera);
    camera->Set(drone.position + glm::vec3(0,5,6), drone.position, camera->up);
    projectionMatrix = glm::perspective(RADIANS(90), window->props.aspectRatio, 0.01f, 200.0f);
	camera->type = 0;

	ortho_camera = new Camera2();
    float maxCoord = 60.f;
    ortho_projectionMatrix = glm::ortho(-maxCoord, maxCoord, -maxCoord, maxCoord, 0.01f, 200.0f);
    ortho_camera->Set(glm::vec3(0, 50, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    ortho_camera->type = 1;


    // ----------------------- Terrain -----------------------
    m_terrain_rows = 40;
    n_terrain_columns = 40;
    terrain = objects2::createTerrain(m_terrain_rows, n_terrain_columns);

    // ----------------------- Obstacles -----------------------
    int nrTrees;
    float x, y, z, width, height;
    nrTrees = rand() % 100 + 50;

    for (int i = 0; i < nrTrees; i++) {
        bool validPosition = false;
        while (!validPosition) {
            x = (rand() % 201) - 100;
            z = (rand() % 201) - 100;
            y = 0;

            width = 1.f;
            height = 5.f;

            if (!isTooCloseToExistingTree(x, z, 5.f) &&
                !isInExcludedZone(x, z, 10.0f)) {
                validPosition = true;
            }
        }

        Mesh* tree_mesh = objects2::createTreeMesh(x, y, z, width, height);
        Tree tree = createTree(x, y, z, width, height);

        trees.push_back(tree);
    }

    int nrHouses;
    nrHouses = rand() % 100 + 50;

    for (int i = 0; i < nrHouses; i++) {
        bool validPosition = false;
        while (!validPosition) {
            x = (rand() % 201) - 100;
            z = (rand() % 201) - 100;
            y = 0;

            width = 2.f;
            height = 5.f;

            if (!isTooCloseToExistingTree(x, z, 5.0f) &&
                !isTooCloseToExistingHouse(x, z, 5.0f) &&
                !isInExcludedZone(x, z, 10.0f)) {
                validPosition = true;
            }
        }

        House house = createHouse(x, y, z, width, height);
        houses.push_back(house);
    }

    // --------------------- Shaders ---------------------
    {
        Shader* shader = new Shader("VertexTema");
        shader->AddShader("src/lab_m1/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
        shader->AddShader("src/lab_m1/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
		Shader* checkshader = new Shader("Checkpoint");
        checkshader->AddShader("src/lab_m1/Tema2/Shaders/CheckpointVertexShader.glsl", GL_VERTEX_SHADER);
        checkshader->AddShader("src/lab_m1/Tema2/Shaders/CheckpointFragmentShader.glsl", GL_FRAGMENT_SHADER);
        checkshader->CreateAndLink();
		shaders[checkshader->GetName()] = checkshader;
    }

    // --------------------- Packet ----------------------


    bool validPosition = false;
    glm::vec3 spawn = glm::vec3(0,0,0);
    glm::vec3 destination = glm::vec3(0, 0, 0);
    while (!validPosition) {
		x = (rand() % 201) - 100;
		z = (rand() % 201) - 100;
		y = 0;

		if (!isTooCloseToExistingTree(x, z, 5.0f) &&    
			!isTooCloseToExistingHouse(x, z, 5.0f) &&
			!isInExcludedZone(x, z, 10.0f)) {
			validPosition = true;
            spawn.x = x;
            spawn.y = y;
            spawn.z = z;
		}
    }
    validPosition = false;
    while (!validPosition) {
        x = (rand() % 201) - 100;
        z = (rand() % 201) - 100;
        y = 0;

        if (!isTooCloseToExistingTree(x, z, 5.0f) &&
            !isTooCloseToExistingHouse(x, z, 5.0f) &&
            !isInExcludedZone(x, z, 10.0f)) {
            validPosition = true;
            destination.x = x;
            destination.y = y;
            destination.z = z;
        }
    }
	float packet_length = 0.5f;
    packet_mesh = objects2::createCube(0, 0, 0, packet_length, glm::vec3(1,0,1));
	spawn = glm::vec3(0, 4, -7);
	destination = glm::vec3(0, 4, 7);

    Mesh* spawn_platform = objects2::CreateParalelipiped(glm::vec3(0,0,0), packet_length * 4.f, packet_length * 4.f, 1.f, glm::vec3(0,0,0.5));
	Mesh* destination_platform = objects2::createCircle(packet_length * 4.f, 1.f, glm::vec3(0, 0.3, 0));
	packet = new Packet(spawn, destination, glm::vec3(0, 4, -7), packet_length, packet_mesh, spawn_platform, destination_platform);
    packetMatrix = glm::mat4(1);
    packetMatrix *= transform3D::Translate(packet->spawn.x, packet->spawn.y, packet->spawn.z);
	spawnmatrix *= transform3D::Translate(spawn.x, spawn.y - packet_length * 2, spawn.z);
	destinationMatrix *= transform3D::Translate(destination.x, destination.y - packet_length * 2.f, destination.z);
	cout << "Score: " << score << endl;

    // ----------------- Checkpoints -----------------
	spawn_checkpoint_mesh = objects2::createCircle(packet_length * 5.f, 500.f, glm::vec3(1, 0, 0));
	destination_checkpoint_mesh = objects2::createCircle(packet_length * 5.f, 500.f, glm::vec3(0, 0, 1));


    // --------------------- Arrow --------------------------
	arrow_mesh = objects2::CreateArrow(0.5f, glm::vec3(1, 0, 0));
}

bool Tema2::isInExcludedZone(float x, float z, float exclusionRadius) {
    float distanceFromCenter = std::sqrt(std::pow(x, 2) + std::pow(z, 2));
    return distanceFromCenter < exclusionRadius;
}

bool Tema2::isTooCloseToExistingTree(float x, float z, float minDistance) {
    for (const Tree& tree : trees) {
        float distance = std::sqrt(std::pow(tree.x - x, 2) + std::pow(tree.z - z, 2));
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

bool Tema2::isTooCloseToExistingHouse(float x, float z, float minDistance) {
    for (const House& house : houses) {
        float distance = std::sqrt(std::pow(house.x - x, 2) + std::pow(house.z - z, 2));
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

glm::vec3 Tema2::getRandomPosition() {
    bool validPosition = false;
    float x, y, z;
    while (!validPosition) {
        x = (rand() % 101) - 50;
        z = (rand() % 101) - 50;
        y = 0.2;

        if (!isTooCloseToExistingTree(x, z, 10.0f) &&
            !isTooCloseToExistingHouse(x, z, 10.0f) &&
            !isInExcludedZone(x, z, 10.0f)) {
            validPosition = true;
        }
        validPosition = true;
    }
    return glm::vec3(x, y, z);
}

m1::Tema2::Tree Tema2::createTree(float x, float y, float z, float width, float height) {
    Mesh* treeMesh = objects2::createTreeMesh(x, y, z, width, height);
    Tree tree(x, y, z, width, height, treeMesh);

    float trunkHeight = height / 3.0f;
    tree.trunkMin = glm::vec3(x - width / 2.0f, y, z - width / 2.0f);
    tree.trunkMax = glm::vec3(x + width / 2.0f, y + trunkHeight, z + width / 2.0f);

    float cone1Height = height / 3.0f;
    tree.cone1Min = glm::vec3(x - width / 1.5f, y + trunkHeight, z - width / 1.5f);
    tree.cone1Max = glm::vec3(x + width / 1.5f, y + trunkHeight + cone1Height, z + width / 1.5f);

    float cone2Height = height / 3.0f;
    tree.cone2Min = glm::vec3(x - width / 1.8f, y + trunkHeight + cone1Height / 1.5f, z - width / 1.8f);
    tree.cone2Max = glm::vec3(x + width / 1.8f, y + trunkHeight + cone1Height + cone2Height, z + width / 1.8f);

    return tree;
}

m1::Tema2::House Tema2::createHouse(float x, float y, float z, float width, float height) {
	Mesh* houseMesh = objects2::createHouseMesh(x, y, z, width, height);
    House house(x, y, z, width, height, houseMesh);

	house.baseMin = glm::vec3(x - width / 2.0f, y, z - width / 2.0f);
	house.baseMax = glm::vec3(x + width / 2.0f, y + height / 2.0f, z + width / 2.0f);

	house.roofMin = glm::vec3(x - width / 3.f, y + height / 2.0f, z - width / 3.f);
	house.roofMax = glm::vec3(x + width / 3.f, y + height, z + width / 3.f);

	return house;
}

bool Tema2::collision_with_tree(glm::vec3 droneMin, glm::vec3 droneMax, Tree tree) {
    if (AABBvsAABB(droneMin, droneMax, tree.trunkMin, tree.trunkMax)) {
        return true;
    }

    if (AABBvsAABB(droneMin, droneMax, tree.cone1Min, tree.cone1Max)) {
        return true;
    }

    if (AABBvsAABB(droneMin, droneMax, tree.cone2Min, tree.cone2Max)) {
        return true;
    }

    return false;
}

bool Tema2::collision_with_house(glm::vec3 droneMin, glm::vec3 droneMax, House house) {
    if (AABBvsAABB(droneMin, droneMax, house.baseMin, house.baseMax)) {
        return true;
    }

    if (AABBvsAABB(droneMin, droneMax, house.roofMin, house.roofMax)) {
        return true;
    }

    return false;
}

bool Tema2::AABBvsAABB(glm::vec3 minA, glm::vec3 maxA, glm::vec3 minB, glm::vec3 maxB) {
    return (
        minA.x <= maxB.x && maxA.x >= minB.x &&
        minA.y <= maxB.y && maxA.y >= minB.y &&
        minA.z <= maxB.z && maxA.z >= minB.z
        );
}

bool Tema2::pickUpPacket(Packet packet, Drone drone) {
	glm::vec3 packetMin = glm::vec3(
		packet.spawn.x - packet.length / 2.0f,
		packet.spawn.y - packet.length / 2.0f,
		packet.spawn.z - packet.length / 2.0f
	);
	glm::vec3 packetMax = glm::vec3(
		packet.spawn.x + packet.length / 2.0f,
		packet.spawn.y + packet.length / 2.0f,
		packet.spawn.z + packet.length / 2.0f
	);

	glm::vec3 droneMin = glm::vec3(
		drone.position.x - drone.length / 8.0f,
		drone.position.y - drone.length / 10.0f,
		drone.position.z - drone.length / 8.0f
	);
	glm::vec3 droneMax = glm::vec3(
		drone.position.x + drone.length / 8.0f,
		drone.position.y + drone.length / 10.0f,
		drone.position.z + drone.length / 8.0f
	);

	return AABBvsAABB(packetMin, packetMax, droneMin, droneMax);
}

bool Tema2::deliveredPacket(Packet *packet, Drone drone) {
    glm::vec2 packetPositionXZ = glm::vec2(packet->destination.x, packet->destination.z);

    glm::vec2 dronePositionXZ = glm::vec2(drone.position.x, drone.position.z);

    float deliveryRadius = packet->length * 4.0f;

    float distance = glm::distance(dronePositionXZ, packetPositionXZ);

    return distance <= deliveryRadius;
}

glm::mat4 Tema2::calculateArrowMatrix(Drone drone, Packet* packet) {
    glm::vec3 dronePosition = camera->GetTargetPosition();

    glm::vec3 targetPosition;
    if (drone.HasPacket)
        targetPosition = packet->destination;
    else
        targetPosition = packet->spawn;

    glm::vec3 direction = glm::normalize(targetPosition - dronePosition);

    glm::vec3 up(0, 1, 0); 
    glm::vec3 axis = glm::cross(up, direction);

    float angle = acos(glm::dot(up, direction));

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, angle, glm::normalize(axis));
    
    return modelMatrix;
}

void Tema2::RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform1f(glGetUniformLocation(shader->program, "frequency"), 0.01f);
    glUniform1f(glGetUniformLocation(shader->program, "amplitude"), 1.f);

    mesh->Render();
}

void Tema2::RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(ortho_camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(ortho_projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform1f(glGetUniformLocation(shader->program, "frequency"), 0.01f);
    glUniform1f(glGetUniformLocation(shader->program, "amplitude"), 1.f);

    mesh->Render();
}

void Tema2::RenderPropeller(float x, float y, float z, const glm::mat4& droneTransform, Camera2* camera) {
    glm::mat4 eliceModelMatrix = glm::mat4(1);
    eliceModelMatrix *= droneTransform;
    eliceModelMatrix *= transform3D::Translate(x - drone.position.x, y - drone.position.y, z - drone.position.z);
    eliceModelMatrix *= transform3D::RotateOY(elice_angularStep);
    elice_angularStep += 0.008f;
    if (camera->type == 0)
        RenderMesh2(elice_mesh, shaders["VertexColor"], eliceModelMatrix);
    else
        RenderMeshOrtho(elice_mesh, shaders["VertexColor"], eliceModelMatrix);
}

void Tema2::RenderScene(Camera2* camera, Packet* packet) {
    glm::mat4 droneTransform = glm::mat4(1);
    droneTransform *= transform3D::Translate(drone.position.x, drone.position.y, drone.position.z);
    droneTransform *= transform3D::Scale(drone.scaleX, drone.scaleY, drone.scaleZ);

    droneTransform *= transform3D::RotateOX(drone.angularStepOX);
    droneTransform *= transform3D::RotateOY(drone.angularStepOY);
    droneTransform *= transform3D::RotateOZ(drone.angularStepOZ);

    if (camera->type == 0)
        RenderMesh2(drone_mesh, shaders["VertexColor"], droneTransform);
    else
    {
        droneTransform *= transform3D::Scale(12.f, 10.f, 12.f);
        RenderMeshOrtho(drone_mesh, shaders["VertexColor"], droneTransform);
    }


    if (!drone.HasPacket && glm::distance(packet->spawn, drone.position) < 1.f)
        drone.HasPacket = true;

    if (packet->health >= 0) {
        if (drone.HasPacket) {

            packet->position = drone.position + glm::vec3(0, -packet->length, 0);

            packetMatrix = glm::mat4(1);

            packetMatrix *= transform3D::Translate(packet->position.x, packet->position.y, packet->position.z);
            packetMatrix *= transform3D::RotateOX(drone.angularStepOX);
            packetMatrix *= transform3D::RotateOY(drone.angularStepOY);
            packetMatrix *= transform3D::RotateOZ(drone.angularStepOZ);

            if (camera->type == 0) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
                RenderMesh2(destination_checkpoint_mesh, shaders["Checkpoint"], destinationMatrix);
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
                RenderMesh2(packet->destination_mesh, shaders["VertexColor"], destinationMatrix);
            }
            else
                RenderMeshOrtho(packet->destination_mesh, shaders["VertexColor"], destinationMatrix);
        }
        else {
            if (camera->type == 0) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
                RenderMesh2(spawn_checkpoint_mesh, shaders["Checkpoint"], spawnmatrix);
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
                RenderMesh2(packet->spawn_mesh, shaders["VertexColor"], spawnmatrix);
            }
            else
                RenderMeshOrtho(packet->spawn_mesh, shaders["VertexColor"], spawnmatrix);
        }
    }
    else {
        packetMatrix = glm::mat4(1);
        packetMatrix *= transform3D::Translate(packet->position.x, packet->position.y, packet->position.z);
        packet->position.y -= 0.01f;

        if (packet->position.y < -3) {
            packet->health = 100;
            packet->position = packet->spawn;
            packetMatrix = glm::mat4(1);
            packetMatrix *= transform3D::Translate(packet->position.x, packet->position.y, packet->position.z);
            drone.HasPacket = false;
        }
    }

    if (drone.HasPacket && deliveredPacket(packet, drone)) {
        score += 10;
        cout << "Score: " << score << endl;
        drone.HasPacket = false;

        packet->spawn = getRandomPosition();

        spawnmatrix = glm::mat4(1);
        spawnmatrix *= transform3D::Translate(packet->spawn.x, packet->spawn.y - packet->length * 2.f, packet->spawn.z);

        packet->destination = getRandomPosition();
        destinationMatrix = glm::mat4(1);
        destinationMatrix *= transform3D::Translate(packet->destination.x, packet->destination.y - packet->length, packet->destination.z);

        packet->position = packet->spawn;

        packetMatrix = glm::mat4(1);
        packetMatrix *= transform3D::Translate(packet->position.x, packet->position.y, packet->position.z);

    }

    if (camera->type == 0)
        RenderMesh2(packet->packet_mesh, shaders["VertexColor"], packetMatrix);
    else
        RenderMeshOrtho(packet->packet_mesh, shaders["VertexColor"], packetMatrix);


    float radical = sqrt(2) / 4.3;
    RenderPropeller(drone.position.x - radical, drone.position.y + drone.length / 8, drone.position.z - radical, droneTransform, camera);
    RenderPropeller(drone.position.x - radical, drone.position.y + drone.length / 8, drone.position.z + radical, droneTransform, camera);
    RenderPropeller(drone.position.x + radical, drone.position.y + drone.length / 8, drone.position.z - radical, droneTransform, camera);
    RenderPropeller(drone.position.x + radical, drone.position.y + drone.length / 8, drone.position.z + radical, droneTransform, camera);
}

void Tema2::RenderTerrain(Drone drone, Camera2* camera) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, -0.5f, 0);
    modelMatrix *= transform3D::Scale(100, 1, 100);
    if (camera->type == 0)
        RenderMesh2(terrain, shaders["VertexTema"], modelMatrix);
    else
        RenderMeshOrtho(terrain, shaders["VertexTema"], modelMatrix);

    modelMatrix = glm::mat4(1);
    for (auto tree : trees) {
        if (camera->type == 0)
            RenderMesh2(tree.mesh, shaders["VertexColor"], modelMatrix);
        else
            RenderMeshOrtho(tree.mesh, shaders["VertexColor"], modelMatrix);
    }
    for (auto house : houses) {
        if (camera->type == 0)
            RenderMesh2(house.mesh, shaders["VertexColor"], modelMatrix);
        else
            RenderMeshOrtho(house.mesh, shaders["VertexColor"], modelMatrix);
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    drone.previous_position = drone.position;
	camera->prev_position = camera->position;

	drone.position.x = camera->GetTargetPosition().x;
	drone.position.y = camera->GetTargetPosition().y;
	drone.position.z = camera->GetTargetPosition().z;

    arrowMatrix = calculateArrowMatrix(drone, packet);

	RenderMesh2(arrow_mesh, shaders["VertexColor"], arrowMatrix);



    RenderTerrain(drone, camera);
    RenderScene(camera, packet);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);    
    
    Camera2 temp_camera(*camera);
	ortho_camera = &temp_camera;
    ortho_camera->type = 1;
	ortho_camera->Set(glm::vec3(drone.position.x - 2, drone.position.y + 20, drone.position.z - 2), glm::vec3(drone.position.x, drone.position.y, drone.position.z), glm::vec3(0, 1, 0));
   
    
    RenderScene(ortho_camera, packet);
    RenderTerrain(drone, ortho_camera);
}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
    float speed = 8.0f;
    float rotationSpeed = 1.0f;

    Camera2 temp_camera(*camera);
    predicted_camera = &temp_camera;

    can_move = 1;

    if (window->KeyHold(GLFW_KEY_W)) {
        predicted_camera->MoveForward(speed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        predicted_camera->MoveForward(-speed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        predicted_camera->TranslateRight(-speed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        predicted_camera->TranslateRight(speed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_R)) {
        predicted_camera->position.y += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_F)) {
        predicted_camera->position.y -= speed * deltaTime;
    }

    if(window->KeyHold(GLFW_KEY_LEFT))
		drone.angularStepOY += rotationSpeed * deltaTime;

	if (window->KeyHold(GLFW_KEY_RIGHT))
		drone.angularStepOY -= rotationSpeed * deltaTime;

	glm::vec3 predicted_pos = predicted_camera->GetTargetPosition();
    glm::vec3 droneMin = glm::vec3(predicted_pos.x - drone.length / 10.f,
        predicted_pos.y - drone.length / 8.0f,
        predicted_pos.z - drone.length / 8.0f);

    glm::vec3 droneMax = glm::vec3(predicted_pos.x + drone.length / 10.f,
        predicted_pos.y + drone.length / 8.0f,
        predicted_pos.z + drone.length / 8.0f);

	if (predicted_camera->position.y < 0.5f)
    {
        predicted_camera->position.y = 0.5f;
        camera->Set(predicted_camera->position, predicted_camera->GetTargetPosition(), predicted_camera->up);
    }

	if (predicted_camera->position.x > 100 || predicted_camera->position.x < -100 ||
		predicted_camera->position.z > 100 || predicted_camera->position.z < -100) {
		can_move = 0;
	}

	if (predicted_camera->GetTargetPosition().x > 100 || predicted_camera->GetTargetPosition().x < -100 ||
		predicted_camera->GetTargetPosition().z > 100 || predicted_camera->GetTargetPosition().z < -100) {
		can_move = 0;
	}

    bool collision_detected = false;
    if (predicted_camera->GetTargetPosition().y < 0.5f) {
        can_move = 0;
    }


    for (Tree tree : trees) {
        if (collision_with_tree(droneMin, droneMax, tree)) {
            collision_detected = true;
            can_move = 0;
            break;
        }
    }

    for (House house : houses) {
        if (collision_with_house(droneMin, droneMax, house)) {
            collision_detected = true;
            can_move = 0;
            break;
        }
    }

    if (can_move == 1) {
        camera->Set(predicted_camera->position, predicted_camera->GetTargetPosition(), predicted_camera->up);
    } else if (collision_detected && drone.HasPacket && camera->GetTargetPosition() != last_hit_position) {
		last_hit_position = camera->GetTargetPosition();
        packet->health -= 10;
        cout << "Packet health: " << packet->health << endl;
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->RotateThirdPerson_OY(-rotationSpeed * deltaTime);
        drone.angularStepOY -= rotationSpeed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        camera->RotateThirdPerson_OY(rotationSpeed * deltaTime);
        drone.angularStepOY += rotationSpeed * deltaTime;
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
        camera->RotateThirdPerson_OX(-deltaY * 0.002f);
		camera->RotateThirdPerson_OY(-deltaX * 0.002f);
	}
}

void Tema2::FrameEnd()
{

}
