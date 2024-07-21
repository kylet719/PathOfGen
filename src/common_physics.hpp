#pragma once

#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

extern bool keymap[512];
const float BBOX_SCALE = 1.02; // slightly reduces player bbox size

struct BBox {
	float top;
	float bottom;
	float left;
	float right;
};

class CommonPhysics {

public:

	virtual void step(float elapsed_ms) = 0;
	BBox getBBoxBounds(Entity& e);

protected:

	void playerMovementHandler(foregroundMotion& player_motion, float elapsed_ms);
	bool checkCircleCollision(Entity& e1, Entity& e2);
	bool checkBoxCollision(Entity& e1, Entity& e2);
	bool checkMeshCollision(Entity& meshEntity, Entity& otherEntity);
	bool isColliding(Entity& meshEntity, Entity& e2);
	float lerp(float property, float end_x, float t);
	vec2 getBezierPosition(BezierCurve& b);
	vec2 get_bounding_box(Entity& entity);
	vec4 get_bbox_corners(vec2& bbox, vec2& position);

private:

	void setAngleToPlayer(foregroundMotion& player_motion, foregroundMotion& enemy_motion);
	vec2 get_position(Entity& entity);
	vec2 get_scale(Entity& entity);
	
};
