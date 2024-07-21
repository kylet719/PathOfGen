#include "mg4_physics.hpp"
#include <iostream>
#include <cstdlib>
#include <components.hpp>
#include <SDL_mixer.h>

unsigned int spawningCounter = 1;
float aliveTime = 0;
const unsigned int MOLE_UPDATE_RATE = 1000; // Smaller is faster spawning

void MiniGame4Physics::step(float elapsed_ms) {
	Entity& player = registry.players.entities[0];
	foregroundMotion& player_motion = registry.foregroundMotions.get(player);

	float step_seconds = elapsed_ms / 1000.f;
	CommonPhysics::playerMovementHandler(player_motion, elapsed_ms);

	moveMole();
	handleVirusCollision();

	for (Entity& ironEntity : registry.consumables.entities) {
		if (checkBoxCollision(ironEntity, player)) {
			registry.collisions.emplace(ironEntity, player);
		}
		handleIronMotion(ironEntity, step_seconds);
	}

	spawningCounter += elapsed_ms;
	if (spawningCounter > MOLE_UPDATE_RATE) {
		spawningCounter = 1;
		MiniGame4Physics::activateAMole();
	}
}

// Potentially can move/restructure into an AI
void MiniGame4Physics::activateAMole() {
	
	std::vector<Entity> &whackAMoleEntities = registry.whackAMole.entities;
	int random = 0+(rand() % whackAMoleEntities.size());
	if (!registry.whackAMole.get(whackAMoleEntities[random]).active && !registry.whackAMole.get(whackAMoleEntities[random]).exploded) {
		registry.whackAMole.get(whackAMoleEntities[random]).active = true;
		registry.collidables.emplace(whackAMoleEntities[random]);
	}
	
}

void MiniGame4Physics::moveMole() {
	std::vector<Entity>& whackAMoleEntities = registry.whackAMole.entities;
	for (unsigned int i = 0; i < whackAMoleEntities.size(); i++) {
		WhackAMole& whackAMoleComponent = registry.whackAMole.get(whackAMoleEntities[i]);
		// If not whacked, move mole into place
		if (whackAMoleComponent.active && !whackAMoleComponent.whacked && !whackAMoleComponent.exploded) {
			if (registry.foregroundMotions.get(whackAMoleEntities[i]).position.y > whackAMoleComponent.origin.y - 90) {
				registry.foregroundMotions.get(whackAMoleEntities[i]).position.y -= 2;
			}
		}else if (whackAMoleComponent.active && whackAMoleComponent.whacked && !whackAMoleComponent.exploded) {
			if (registry.foregroundMotions.get(whackAMoleEntities[i]).position.y < whackAMoleComponent.origin.y) {
				registry.foregroundMotions.get(whackAMoleEntities[i]).position.y += 2;
			}
			if (registry.foregroundMotions.get(whackAMoleEntities[i]).position.y >= whackAMoleComponent.origin.y) {
				whackAMoleComponent.whacked = false;
				whackAMoleComponent.active = false;
			}
		}
	}
	
}

void MiniGame4Physics::handleVirusCollision() {
	bool addingCollision = 0; // Different from other collisions because it's an "Is colliding or not colliding" logic
	 //Check for collisions between all moving entities and moving entities with staticObjects
	 //For each entity in `collidables`, check against all other collidables
	 //If there is a collision, add to the collisions container
	Entity player = registry.players.entities[0];
	ComponentContainer<Collidable>& collidableContainer = registry.collidables;

	for (uint i = 0; i < collidableContainer.components.size(); i++)
	{
		Collidable& collidable_i = collidableContainer.components[i];
		Entity& entity_i = collidableContainer.entities[i];

		// in Mini Game
		bool oneIsPlayer = entity_i == registry.players.entities[0] || player == registry.players.entities[0];
		bool oneIsVirus = registry.whackAMole.has(entity_i) || registry.whackAMole.has(player);

		// check that one is the player and one is a virus and that they are colliding
		if (oneIsPlayer && oneIsVirus && checkCircleCollision(entity_i, player)) {
			registry.collisions.emplace_with_duplicates(entity_i, player);
			registry.collisions.emplace_with_duplicates(player, entity_i);
			addingCollision = 1; 
		}
		
		if (!addingCollision) {
			registry.collisions.clear();
		}
	}

}

void randomizeBezierPoints_MG4(BezierCurve& bezier, foregroundMotion& motion) {
	// randomize start/end points
	int side = rand() % 2;
	vec2 startPos;
	vec2 endPos;
	if (side == 0) { // start on left; end on right
		startPos = vec2(-motion.scale.x, rand() % window_height_px);
		endPos = vec2(window_width_px + motion.scale.x, rand() % window_height_px);
	} else { // start on right; end on left
		startPos = vec2(window_width_px + motion.scale.x, rand() % window_height_px);
		endPos = vec2(-motion.scale.x, rand() % window_height_px);
	}

	motion.position = startPos;
	
	vec2 p1 = vec2(rand() % window_width_px, rand() % window_height_px);
	vec2 p2 = vec2(rand() % window_width_px, rand() % window_height_px);

	bezier.points = { startPos, p1, p2, endPos };
}

void MiniGame4Physics::handleIronMotion(Entity& ironEntity, float step_seconds) {
	foregroundMotion& motion = registry.foregroundMotions.get(ironEntity);
	BezierCurve& bezier = registry.beziers.get(ironEntity);
	bezier.t = fmin(bezier.t + step_seconds / 5.f, 1.0f);

	motion.position = getBezierPosition(bezier);

	if (bezier.t == 1.0f) {
		randomizeBezierPoints_MG4(bezier, motion);
		bezier.t = 0.0f;

		if (!registry.foregroundRenderRequests.has(ironEntity)) {
			registry.foregroundRenderRequests.insert(
				ironEntity,
				{
					TEXTURE_ASSET_ID::IRON,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE 
				}
			);
		}
	}
}