#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

#include "Camera.hpp"
#include "Physics.hpp"
#include "Gun.hpp"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream> 

JPH_SUPPRESS_WARNINGS

class PlayerController {
public:
    PlayerController(glm::vec3 startPosi, Physics& inPhysics);

    ~PlayerController() = default;
    void update(GLFWwindow* window, double deltaTime);
    void processMouse(double xpos, double ypos);

    bool isGrounded();

    bool firstMouse = true;
    double lastX = 0.0f, lastY = 0.0f;
    double yaw = -90.0f;
    float pitch = 0.0f;

    glm::vec3 startPos = glm::vec3(0.0f, 10.0f, 0.0f);

    glm::vec3 position = startPos;
    glm::mat4 getViewMatrix() const;

    float currentFov = 80.0f;
private:
    
    Physics& physics;
    Camera camera;
    Gun gun;

    float moveSpeed = 2.5f;
	float forwardSpeed = 2.5f;
	float runSpeed = 5.0f;

    float mouseSensitivity = 0.1f;

    bool isJumping = false;
    float jumpVelocity;


    JPH::BodyID playerBodyID;
	JPH::BodyID groundSensorBodyID;

    JPH::ShapeRefC mPlayerShape;

    float mPlayerHeight = 1.8f; // meters
    float mPlayerRadius = 0.3f; // meters

    float mGroundSensorRadius = 0.1f; // meters

	float walkFov = 80.0f;
    float runningFov = 83.0f;
    float runningFovMultiplier = 1.0f;
};

#endif // PLAYERCONTROLLER_HPP
