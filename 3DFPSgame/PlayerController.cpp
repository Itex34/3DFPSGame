#include "PlayerController.hpp"

PlayerController::PlayerController(glm::vec3 startPosi, Physics& inPhysics)
    : startPos(startPosi),
    isJumping(false),
    jumpHeight(1.0f),
    camera(startPosi),
    physics(inPhysics){

    //Get the BodyInterface
    JPH::BodyInterface& bodyInterface = physics.getPhysicsSystem().GetBodyInterface();

    //Create a collision shape


    mPlayerShape = new JPH::CapsuleShape(mPlayerHeight * 0.5f, mPlayerRadius);
	JPH::ShapeRefC groundSensorShape = new JPH::SphereShape(mGroundSensorRadius);

    //Create body settings
    JPH::BodyCreationSettings playerBodySettings(
        mPlayerShape,
        JPH::RVec3(startPos.x, startPos.y, startPos.z),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Dynamic,
        Layers::MOVING // Use the moving layer
    );

    JPH::BodyCreationSettings groundSensorSettings(
        groundSensorShape,
        JPH::RVec3(startPos.x, startPos.y, startPos.z),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Kinematic,
        Layers::MOVING
    );

	//Set the ground sensor to be a trigger
    groundSensorSettings.mIsSensor = true;
    //Make player not rotate
    playerBodySettings.mAllowSleeping = false;
    playerBodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;
    playerBodySettings.mInertiaMultiplier = 0.0f; // Disable rotations

    // Create and add the bodies to the world
    playerBodyID = bodyInterface.CreateAndAddBody(playerBodySettings, JPH::EActivation::Activate);
    groundSensorBodyID = bodyInterface.CreateAndAddBody(groundSensorSettings, JPH::EActivation::Activate);

}


bool PlayerController::isGrounded() {
    return true;
}

void PlayerController::update(GLFWwindow* window, double deltaTime) {
    JPH::BodyInterface& bodyInterface = physics.getPhysicsSystem().GetBodyInterface();

    JPH::Vec3 currentVelocity = bodyInterface.GetLinearVelocity(playerBodyID);
    glm::vec3 inputVelocity(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        inputVelocity += camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        inputVelocity -= camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        inputVelocity -= glm::normalize(glm::cross(camera.front, camera.up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        inputVelocity += glm::normalize(glm::cross(camera.front, camera.up));

    //avoid diagonal speed boost
    if (glm::length(inputVelocity) > 0.0f)
        inputVelocity = glm::normalize(inputVelocity) * moveSpeed;

    JPH::Vec3 inputVel(inputVelocity.x, currentVelocity.GetY(), inputVelocity.z);

    //bodyInterface.AddForce(playerBodyID, inputVel, JPH::EActivation::Activate);

    bodyInterface.SetLinearVelocity(playerBodyID, inputVel);

    // Jump
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping) {
        bodyInterface.SetLinearVelocity(playerBodyID, JPH::Vec3(
            currentVelocity.GetX(),
            jumpHeight,
            currentVelocity.GetZ()
        ));
        isJumping = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        isJumping = false;

    // Update the camera position from the body
    JPH::RVec3 playerPos = bodyInterface.GetPosition(playerBodyID);
    JPH::RVec3 sensorPos = playerPos - JPH::Vec3(0, mPlayerHeight * 0.5f + 0.05f, 0); // a bit under the feet
    bodyInterface.SetPosition(groundSensorBodyID, sensorPos, JPH::EActivation::DontActivate);
    camera.position = glm::vec3(playerPos.GetX(), playerPos.GetY(), playerPos.GetZ());

	position.x = playerPos.GetX();
	position.y = playerPos.GetY();
	position.z = playerPos.GetZ();
}



/*
void PlayerController::update(GLFWwindow* window, double deltaTime) {
    JPH::BodyInterface& bodyInterface = physics.getPhysicsSystem().GetBodyInterface();

    // Get current position
    JPH::RVec3 currentPos = bodyInterface.GetPosition(playerBodyID);

    // Prepare movement input
    glm::vec3 inputVelocity(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        inputVelocity += camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        inputVelocity -= camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        inputVelocity -= glm::normalize(glm::cross(camera.front, camera.up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        inputVelocity += glm::normalize(glm::cross(camera.front, camera.up));

    // Normalize to avoid diagonal speed boost
    if (glm::length(inputVelocity) > 0.0f)
        inputVelocity = glm::normalize(inputVelocity) * moveSpeed;

    // Gravity
    static float verticalVelocity = 0.0f;
    verticalVelocity += physics.getPhysicsSystem().GetGravity().GetY() * deltaTime; // apply gravity

    // Jump
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded()) {
        verticalVelocity = jumpHeight;
    }

    // Final movement vector
    glm::vec3 finalMove(
        inputVelocity.x,
        verticalVelocity,
        inputVelocity.z
    );

    // Predict new position
    JPH::Vec3 displacement = JPH::Vec3(finalMove.x, finalMove.y, finalMove.z) * deltaTime;
    JPH::RVec3 newPos = currentPos + displacement;
	JPH::Quat rotation = bodyInterface.GetRotation(playerBodyID);   

    bodyInterface.MoveKinematic(playerBodyID, newPos, rotation, deltaTime);

    // Update the ground sensor under the player
    JPH::RVec3 sensorPos = newPos - JPH::Vec3(0, mPlayerHeight * 0.5f + 0.05f, 0);
    bodyInterface.SetPosition(groundSensorBodyID, sensorPos, JPH::EActivation::DontActivate);

    // Update camera
    camera.position = glm::vec3(newPos.GetX(), newPos.GetY(), newPos.GetZ());

    position.x = newPos.GetX();
    position.y = newPos.GetY();
    position.z = newPos.GetZ();
}
*/
void PlayerController::processMouse(double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    double xoffset = xpos - lastX;
    double yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch -= yoffset;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    camera.updateRotation(yaw, pitch);
}

glm::mat4 PlayerController::getViewMatrix() const {
    return camera.getViewMatrix();
}
