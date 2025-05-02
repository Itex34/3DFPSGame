#pragma once

#include "Physics.hpp"
#include <glm/glm.hpp>
#include <chrono>

class Gun {
public:
	Gun(Physics& inPhysics, JPH::BodyID& inIgnoreBody, float inFireRate, float inReloadTime);
	~Gun() = default;

	void requestFire();
	void update(glm::vec3 rayOrigin, glm::vec3 rayDirection, JPH::BodyID targetBody, double deltaTime);
	void fire(glm::vec3 rayOrigin, glm::vec3 rayDirection, JPH::BodyID targetBody);

	void reload();


	glm::vec3 gunCamOffset = glm::vec3(10.0f, 0.0f, 0.0f);
	glm::vec3 hitPoint = glm::vec3(0.0f, 0.0f, 0.0f);
private:
	JPH::BodyID& ignoreBody;
	Physics& physics;



	JPH::RayCastSettings raySettings;

	JPH::BroadPhaseLayerFilter broadPhaseLayerFilter;
	JPH::ObjectLayerFilter objectLayerFilter;

	static constexpr float maxShootDistance = 100000.0f;


	float reloadTime = 3.0f;// seconds
	float fireRate = 0.2f;// seconds per shot
	float timeSinceLastShot = 0.0f;
	bool canShoot = true;
	bool wantsToFire = false;


	bool isReloading = false;
	float reloadTimer = 0.0f;
	unsigned int maxAmmo = 30;
	unsigned int currentAmmo = maxAmmo;
};
