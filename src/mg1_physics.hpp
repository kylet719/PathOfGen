#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

class MiniGame1Physics : public CommonPhysics {
	const float T_PARAMETER = 0.03; // Changes the rate of interpolation to new set value
	const float ATP_MIN_X_SCALE = 20;
	const float ATP_MAX_X_SCALE = 100;
	const float ATP_MIN_Y_SCALE = -40;
	const float ATP_MAX_Y_SCALE = -90;
	const float DETECTION_OFFSET = 20; // Offet to detect when to change interpolation from increasing to decreasing
public:

	void step(float elapsed_ms);
	MiniGame1Physics() {}

private:
	void consumableMovementHandler(float elapsed_ms);
	void handleATPCollisions();
	void playerMovementHandlerMG1(foregroundMotion& player_motion, float elapsed_ms);

	bool increasing_x_scale = true;
	bool increasing_y_scale = true;
};
