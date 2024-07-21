#include "mg2_physics.hpp"

const int BACKGROUND_SPEED = 200;
float countup_timer = 0;

void MiniGame2Physics::step(float elapsed_ms) {
	float step_seconds = elapsed_ms / 1000.f;
	countup_timer += step_seconds;
	Entity& player = registry.players.entities[0];
	foregroundMotion& player_motion = registry.foregroundMotions.get(player);

	CommonPhysics::playerMovementHandler(player_motion, elapsed_ms);
	
	for (int i = 0; i < registry.deadlys.entities.size(); i++) {
		Entity& deadlyMesh = registry.deadlys.entities[i];

		foregroundMotion& mesh_motion = registry.foregroundMotions.get(deadlyMesh);
		mesh_motion.position -= vec2{ BACKGROUND_SPEED * step_seconds, sin(countup_timer) * step_seconds * 50.f};
		fancyMeshMotion(deadlyMesh, step_seconds);

		if (isColliding(deadlyMesh, player)) {
			registry.collisions.emplace_with_duplicates(player, deadlyMesh);
			registry.collisions.emplace_with_duplicates(deadlyMesh, player);
		}

		// Delete entities that fall outside of screen
		if (mesh_motion.position.x <= -100.0f) registry.remove_all_components_of(deadlyMesh);
	}

	for (auto lipid : registry.consumables.entities) {
		foregroundMotion& lipid_motion = registry.foregroundMotions.get(lipid);
		lipid_motion.position.x -= BACKGROUND_SPEED * step_seconds + sin(countup_timer*5)/5;
	}

	player_motion.position.y = max(200.f, min((float)window_height_px - 150, player_motion.position.y));
	
	moveBackground(step_seconds);
	handleLipidCollisions();
}

void initMG2Physics() {
	countup_timer = 0;
}

void MiniGame2Physics::fancyMeshMotion(Entity& meshEntity, float step_seconds) {
	foregroundMotion& mesh_motion = registry.foregroundMotions.get(meshEntity);
	Random& r = registry.random.get(meshEntity);
	int meshIndex = registry.meshPtrs.get(meshEntity)->index;

	switch (meshIndex) {
	case 0:
		mesh_motion.position.x -= 100 * cos(countup_timer * 5 * r.random) * step_seconds;
		mesh_motion.position.y += 20 * sin(countup_timer * 5 * r.random) * step_seconds;
		break;
	case 1:
		mesh_motion.position.x -= cos(countup_timer) * 20 * step_seconds;
		mesh_motion.scale.y = 500 + 100 * abs(sin(10 * countup_timer * r.random));
		break;
	case 2:
		mesh_motion.angle += countup_timer/300;
		mesh_motion.position.y = (window_height_px - 200) - 150 * (abs(0.5 * sin(3 * countup_timer)));
		mesh_motion.position.x -= BACKGROUND_SPEED * step_seconds * r.random;
		break;
	default:
		break;
	}
}

void MiniGame2Physics::moveBackground(float step_seconds) {
	registry.backgroundMotions.get(registry.background.entities[0]).position.x -= BACKGROUND_SPEED * step_seconds;
	registry.backgroundMotions.get(registry.background.entities[1]).position.x -= BACKGROUND_SPEED * step_seconds;
	if (registry.backgroundMotions.get(registry.background.entities[0]).position.x <= -window_width_px/2) {
		registry.backgroundMotions.get(registry.background.entities[0]).position.x = window_width_px + window_width_px / 2;
	}
	if (registry.backgroundMotions.get(registry.background.entities[1]).position.x <= -window_width_px / 2) {
		registry.backgroundMotions.get(registry.background.entities[1]).position.x = window_width_px + window_width_px / 2;
	}
}

void MiniGame2Physics::handleLipidCollisions() {

	// Check for collisions between all moving entities and moving entities with staticObjects
	// For each entity in `collidables`, check against all other collidables
	// If there is a collision, add to the collisions container
	ComponentContainer<foregroundMotion>& motion_container = registry.foregroundMotions;
	ComponentContainer<Collidable>& collidableContainer = registry.collidables;
	ComponentContainer<backgroundMotions>& staticObjectContainer = registry.backgroundMotions;

	for (uint i = 0; i < collidableContainer.components.size(); i++)
	{
		Collidable& collidable_i = collidableContainer.components[i];
		Entity& entity_i = collidableContainer.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < collidableContainer.components.size(); j++)
		{
			Collidable& collidable_j = collidableContainer.components[j];
			Entity& entity_j = collidableContainer.entities[j];

			// in Mini Game
			bool oneIsPlayer = entity_i == registry.players.entities[0] || entity_j == registry.players.entities[0];
			bool oneIsConsumable = registry.consumables.has(entity_i) || registry.consumables.has(entity_j);

			// check that one is the player and one is a consumable and that they are colliding
			if (oneIsPlayer && oneIsConsumable && checkCircleCollision(entity_i, entity_j)) {
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}
}
