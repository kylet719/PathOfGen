#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

class CreditsPhysics : public CommonPhysics {

public:

	void step(float elapsed_ms);
	CreditsPhysics() {}

private:
	void screenMoves_panDown(float elapsed_time);
	void moveAnimation(float elapsed_ms);
};