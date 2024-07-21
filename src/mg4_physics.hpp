#pragma once

#include "common_physics.hpp"
#include "tiny_ecs_registry.hpp"

void randomizeBezierPoints_MG4(BezierCurve& bezier, foregroundMotion& motion);

class MiniGame4Physics : public CommonPhysics {

public:
	void step(float elapsed_ms);
	MiniGame4Physics() {}

private:
	void activateAMole();
	void moveMole();
	void handleVirusCollision();
	void handleIronMotion(Entity& ironEntity, float step_seconds);
};