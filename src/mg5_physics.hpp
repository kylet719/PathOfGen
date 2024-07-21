#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_init.hpp"

const float PADDLE_STEP = 15;

class MiniGame5Physics : public CommonPhysics {
	
public:
	void step(float elapsed_ms);
	MiniGame5Physics() {}

private:
	void checkForBounce(Entity& ballEntity);
	void paddleMovementHandler(foregroundMotion& paddleMotion, float elapsed_ms);
	void checkForCollisions(Entity& ballEntity);
	void handlePaddleCollision(Entity& paddleEntity, Entity& ballEntity);
	void handleBrickCollision(Entity& brickEntity, Entity& ballEntity);
	void handleOxygenMotion(Entity& oxygenEntity, float step_seconds);
	void removeOffScreen(Entity& entity);

};
