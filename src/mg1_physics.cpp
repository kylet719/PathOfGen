#include "mg1_physics.hpp"

void MiniGame1Physics::step(float elapsed_ms) {
	Entity& player = registry.players.entities[0];
	Entity& enemy = registry.deadlys.entities[0];
	foregroundMotion& player_motion = registry.foregroundMotions.get(player);
	foregroundMotion& enemy_motion = registry.foregroundMotions.get(enemy);

	// check for collision between Gen and evil virus
	if (checkCircleCollision(player, enemy)) {
		registry.collisions.emplace_with_duplicates(player, enemy);
		registry.collisions.emplace_with_duplicates(enemy, player);
	}

	//key handling for player
	playerMovementHandlerMG1(player_motion, elapsed_ms);

	// handle consumable movement
	consumableMovementHandler(elapsed_ms);

	handleATPCollisions();
}

void MiniGame1Physics::consumableMovementHandler(float elapsed_ms) {
	ComponentContainer<Consumable>& consumableContainer = registry.consumables;

	for (int i = 0; i < consumableContainer.components.size(); i++) {
		Entity& entity = consumableContainer.entities[i];
		foregroundMotion& motion = registry.foregroundMotions.get(entity);

		// Allows for scale variation and back and forth motion using linear interpolation. 
		// Allows for hard-coded rotation of the ATPs
		float new_x = 0.0f;
		float new_scale = 0.0f;
		float target_x;

		float x_scale_change = 0;
		if (increasing_x_scale)
		{
			if (motion.scale.x >= ATP_MAX_X_SCALE - DETECTION_OFFSET)
			{
				increasing_x_scale = false;
				x_scale_change = lerp(motion.scale.x, ATP_MIN_X_SCALE, T_PARAMETER);
			}	
			else
				x_scale_change = lerp(motion.scale.x, ATP_MAX_X_SCALE, T_PARAMETER);
		}
		else
		{
			if (motion.scale.x <= ATP_MIN_X_SCALE + DETECTION_OFFSET)
			{
				increasing_x_scale = true;
				x_scale_change = lerp(motion.scale.x, ATP_MAX_X_SCALE, T_PARAMETER);
			}
			else
				x_scale_change = lerp(motion.scale.x, ATP_MIN_X_SCALE, T_PARAMETER);
		}
		motion.scale.x = x_scale_change;

		float y_scale_change = 0;
		if (increasing_y_scale)
		{
			if (motion.scale.y <= ATP_MAX_Y_SCALE + DETECTION_OFFSET)
			{
				increasing_y_scale = false;
				y_scale_change = lerp(motion.scale.y, ATP_MIN_Y_SCALE, T_PARAMETER);
			}
			else
				y_scale_change = lerp(motion.scale.y, ATP_MAX_Y_SCALE, T_PARAMETER);
		}
		else
		{
			if (motion.scale.y >= ATP_MIN_Y_SCALE - DETECTION_OFFSET)
			{
				increasing_y_scale = true;
				y_scale_change = lerp(motion.scale.y, ATP_MAX_Y_SCALE, T_PARAMETER);
			}
			else
				y_scale_change = lerp(motion.scale.y, ATP_MIN_Y_SCALE, T_PARAMETER);
		}

		motion.scale.y = y_scale_change;
	}
}

void MiniGame1Physics::handleATPCollisions() {

	// Check for collisions between all moving entities and moving entities with staticObjects
	// For each entity in `collidables`, check against all other collidables
	// If there is a collision, add to the collisions container
    ComponentContainer<foregroundMotion> &motion_container = registry.foregroundMotions;
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

void MiniGame1Physics::playerMovementHandlerMG1(foregroundMotion& player_motion, float elapsed_ms) {
	float step_seconds = elapsed_ms / 1000.f;
	float new_pos = 0;
	int sprite_offset = 35;
	for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
		if (keymap[i]) {
			switch (i) {
			case GLFW_KEY_W:
				new_pos = player_motion.position.y - (275 * step_seconds);
				if (!collidesWithWallY({ player_motion.position.x, new_pos - 50 }, sprite_offset))
					player_motion.position.y += -275 * step_seconds;
				break;
			case GLFW_KEY_S:
				new_pos = player_motion.position.y + (275 * step_seconds);
				if (!collidesWithWallY({ player_motion.position.x, new_pos + 50 }, sprite_offset))
					player_motion.position.y += 275 * step_seconds;
				break;
			case GLFW_KEY_A:
				new_pos = player_motion.position.x - (275 * step_seconds);
				if (!collidesWithWallX({ new_pos - 50, player_motion.position.y }, sprite_offset))
					player_motion.position.x += -275 * step_seconds;
				break;
			case GLFW_KEY_D:
				new_pos = player_motion.position.x + (275 * step_seconds);
				if (!collidesWithWallX({ new_pos + 50, player_motion.position.y }, sprite_offset))
					player_motion.position.x += 275 * step_seconds;
				break;
			}
		}
		// Bounds the player in the game screen
		player_motion.position.x = max(50.f, min((float)window_width_px - 50, player_motion.position.x));
		player_motion.position.y = max(50.f, min((float)window_height_px - 50, player_motion.position.y));
	}
}
