#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

class TitlePhysics : public CommonPhysics {

public:

	void step(float elapsed_ms);
	TitlePhysics() {}

private:
	bool panUp = true;
	void screenMoves_panUpwards(float step_seconds);
	void screenMoves_panDown(float step_seconds);
	void title_pans_down(float step_seconds);
	void selectionKeyHandler();
};