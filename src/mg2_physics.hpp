#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

class MiniGame2Physics : public CommonPhysics {
	
public:
	void step(float elapsed_ms);
	MiniGame2Physics() {}

private:
	void moveBackground(float step_seconds);
	void handleLipidCollisions();
	void fancyMeshMotion(Entity& meshEntity, float step_seconds);
};

void initMG2Physics();