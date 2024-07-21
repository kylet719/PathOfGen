#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

extern bool on_platform;
extern float gravity_factor;
extern int jump_count;

class MiniGame3Physics : public CommonPhysics {

public:
	void step(float elapsed_ms);
	MiniGame3Physics() {}

private:
	
	int POS_STEP = 5;
	bool disable_a = false;
	bool disable_d = false;
	Entity player_on_platform;
	bool is_jumping = false;
	float jump_speed = 20;
	float random_dir = 1;

	void glucose_movement(float elapsed_ms);
	void handlePlayerMovement(float elapsed_ms, Entity player_entity, foregroundMotion& player_motion);
	void collisionDetection();
	void handlePlatformCollisions(Entity& player, foregroundMotion& player_motion);
	void handleOffPlatformSideCollisions(Entity& player, foregroundMotion& player_motion);
	void checkOffPlatform(Entity& entity, foregroundMotion& player_motion);
	bool playerFinish(Entity& entity_i, Entity& entity_j);
};