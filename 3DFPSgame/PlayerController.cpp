#include "PlayerController.hpp"

PlayerController::PlayerController(glm::vec3 startPosi, Physics& inPhysics)
    : startPos(startPosi),
    isJumping(false),
    jumpVelocity(4.0f),
    camera(startPosi),
    physics(inPhysics),
    gun(inPhysics, playerBodyID, 0.2f, 3.0f){

    JPH::BodyInterface& bodyInterface = physics.getPhysicsSystem().GetBodyInterface();

    mPlayerShape = new JPH::CapsuleShape(mPlayerHeight * 0.5f, mPlayerRadius);

    JPH::BodyCreationSettings playerBodySettings(
        mPlayerShape,
        JPH::RVec3(startPos.x, startPos.y, startPos.z),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Dynamic,
        Layers::MOVING // Use the moving layer
    );
    playerBodySettings.mAllowSleeping = false;
    playerBodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;
    playerBodySettings.mInertiaMultiplier = 0.0f; // disable rotations


    playerBodyID = bodyInterface.CreateAndAddBody(playerBodySettings, JPH::EActivation::Activate);

}


bool PlayerController::isGrounded() {
	JPH::RMat44 comTransform = physics.getPhysicsSystem().GetBodyInterface().GetCenterOfMassTransform(playerBodyID);

    JPH::Vec3 castDir = JPH::Vec3(0, -(mPlayerHeight * 0.5f + 0.1f), 0);

    JPH::RShapeCast shapeCast(
        mPlayerShape,
        JPH::Vec3::sReplicate(1.0f),
        comTransform,
        castDir
    );

	JPH::ShapeFilter shapeFilter;
	JPH::IgnoreSingleBodyFilter bodyFilter(playerBodyID);// ignore the player body itself
	JPH::ObjectLayerFilter objectLayerFilter;
	JPH::BroadPhaseLayerFilter broadPhaseLayerFilter;

    
    JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;
	JPH::ShapeCastSettings settings;
    JPH::ShapeCastResult result;

    physics.getPhysicsSystem().GetNarrowPhaseQuery().CastShape(
        shapeCast,
        settings,
        JPH::Vec3::sZero(),
        collector,
        broadPhaseLayerFilter,
        objectLayerFilter,
        bodyFilter, 
        shapeFilter
    );
    
    if (collector.HadHit()) {
        float hitFraction = collector.mHit.mFraction;
        return hitFraction < 0.05f; // only count as grounded if it's very close
    }

    return false;
}

void PlayerController::update(GLFWwindow* window, double deltaTime) {

    JPH::BodyInterface& bodyInterface = physics.getPhysicsSystem().GetBodyInterface();
    JPH::Vec3 currentVelocity = bodyInterface.GetLinearVelocity(playerBodyID);


    glm::vec3 inputVelocity(0.0f);



    glm::vec3 moveDir(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDir += camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDir -= camera.XZfront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDir -= glm::normalize(glm::cross(camera.front, camera.up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDir += glm::normalize(glm::cross(camera.front, camera.up));

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    float currentSpeed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? runSpeed : moveSpeed;

    float targetFov = (currentSpeed == runSpeed) ? runningFov * runningFovMultiplier : walkFov;
    float fovSmoothSpeed = 10.0f;
    currentFov += (targetFov - currentFov) * fovSmoothSpeed * deltaTime;

    inputVelocity = moveDir * currentSpeed;

    JPH::Vec3 inputVel(inputVelocity.x, currentVelocity.GetY(), inputVelocity.z);

    bool grounded = isGrounded();
    bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;


    if (spacePressed && grounded) {
        inputVel.SetY(jumpVelocity);
    }

    if (grounded && !spacePressed) {
        inputVel.SetY(0); // Cancel falling when on ground, doesn't entirely prevent the player from slowly falling through the florr until Y = 0.3

        //disable gravity when grounded to fix the problem
		physics.getPhysicsSystem().GetBodyInterface().SetGravityFactor(playerBodyID, 0.0f);
    }
    else {
		physics.getPhysicsSystem().GetBodyInterface().SetGravityFactor(playerBodyID, 1.0f);
    }


    //apply the velocity
    bodyInterface.SetLinearVelocity(playerBodyID, inputVel);

    JPH::RVec3 playerPos = bodyInterface.GetPosition(playerBodyID);

    //make the camera follow the player
    camera.position = glm::vec3(playerPos.GetX(), playerPos.GetY(), playerPos.GetZ());

    position.x = playerPos.GetX();
    position.y = playerPos.GetY();
    position.z = playerPos.GetZ();


	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        gun.reload();
	}

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        gun.requestFire();
    }
	gun.update(camera.position, camera.front, physics.floorBodyID, deltaTime);
}

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
