#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

class OrganPhysics : public CommonPhysics {

public:

	void step(float elapsed_ms);
	OrganPhysics() {}

private:
	void arrowMovementHandler(float elapsed_ms);
	void handleNodeCollisions();
	void playerMovementHandlerOrgans(foregroundMotion& player_motion, float elapsed_ms);
	bool checkOrganWallX(vec2 pos, int offset);
	bool checkOrganWallY(vec2 pos, int offset);
};
