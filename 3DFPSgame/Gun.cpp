#include "Gun.hpp"

Gun::Gun(Physics& inPhysics, JPH::BodyID& inIgnoreBody, float inFireRate, float inReloadTime) :
    physics(inPhysics),
    ignoreBody(inIgnoreBody),
	fireRate(inFireRate),
    reloadTime(inReloadTime){

}



void Gun::fire(glm::vec3 rayOrigin, glm::vec3 rayDirection, JPH::BodyID targetBody) {

    glm::vec3 normDirection = glm::normalize(rayDirection);


    JPH::RRayCast rayCast(
        JPH::RVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
        JPH::RVec3(normDirection.x, normDirection.y, normDirection.z) * maxShootDistance
    );

    JPH::IgnoreSingleBodyFilter bodyFilter(ignoreBody);
    JPH::RayCastResult rayResult;

    if (physics.getPhysicsSystem().GetNarrowPhaseQuery().CastRay(
        rayCast, rayResult, broadPhaseLayerFilter, objectLayerFilter, bodyFilter
    )) {
        if (rayResult.mBodyID == targetBody) {
            std::cout << "Hit the body: " << rayResult.mBodyID.GetIndexAndSequenceNumber() << "\n";
        }
        else {
            std::cout << "Hit something else: " << rayResult.mBodyID.GetIndexAndSequenceNumber() << "\n";
        }
    }
    else {
        std::cout << "Raycast hit nothing.\n";
    }
    
    hitPoint = rayOrigin + normDirection * (rayResult.mFraction * maxShootDistance);
}

void Gun::update(glm::vec3 rayOrigin, glm::vec3 rayDirection, JPH::BodyID targetBody, double deltaTime) {
    if (isReloading) {
        reloadTimer += deltaTime;
        if (reloadTimer >= reloadTime) {
            currentAmmo = maxAmmo;
            isReloading = false;
            reloadTimer = 0.0f;
            std::cout << "Reload complete.\n";
        }
        return; // can't shoot while reloading
    }

    timeSinceLastShot += deltaTime;

    if (wantsToFire && timeSinceLastShot >= fireRate && currentAmmo > 0) {
        fire(rayOrigin, rayDirection, targetBody);
        timeSinceLastShot = 0.0f;
        currentAmmo--;

        if (currentAmmo == 0) {
            reload();
        }
    }

    wantsToFire = false;
    std::cout << "Current ammo: " << currentAmmo << "\n";
}

void Gun::requestFire() {
    wantsToFire = true;
}

void Gun::reload() {
    if (!isReloading && currentAmmo < maxAmmo) {
        std::cout << "Reloading...\n";
        isReloading = true;
        reloadTimer = 0.0f;
    }
}
