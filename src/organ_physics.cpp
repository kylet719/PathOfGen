#include "organ_physics.hpp"

void OrganPhysics::step(float elapsed_ms) {
	if (registry.players.entities.size() > 0) {
		Entity& player = registry.players.entities[0];
		foregroundMotion& player_motion = registry.foregroundMotions.get(player);

	arrowMovementHandler(elapsed_ms);
	
	playerMovementHandlerOrgans(player_motion, elapsed_ms);

		handleNodeCollisions();
	}

}

void OrganPhysics::arrowMovementHandler(float elapsed_ms) {
	auto& arrowContainer = registry.arrows;

	for (int i = 0; i < arrowContainer.components.size(); i++) {
		Entity& arrowEntity = arrowContainer.entities[i];
		backgroundMotions& arrowMotion = registry.backgroundMotions.get(arrowEntity);
		backgroundMotions& nodeMotion = registry.backgroundMotions.get(registry.arrows.get(arrowEntity).associatedNode);

		if (arrowMotion.position.y >= nodeMotion.position.y - 50) {
			arrowMotion.velocity.y = -50.f;
		}
		else if (arrowMotion.position.y <= nodeMotion.position.y - 75) {
			arrowMotion.velocity.y = 50.f;
		}

		float step_seconds = elapsed_ms / 1000.f;
		arrowMotion.position += arrowMotion.velocity * step_seconds;
	}
}

void OrganPhysics::handleNodeCollisions() {
	ComponentContainer<Collidable>& collidableContainer = registry.collidables;
	Entity player = registry.players.entities[0];
	for (uint i = 0; i < collidableContainer.components.size(); i++)
	{
		Entity& entity_i = collidableContainer.entities[i];
		if (entity_i != player) {
			if (game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
				game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
				game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
				game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
				game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
				game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
				game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
				)
			{
				if (checkCircleCollision(entity_i, player))
				{
					// Only add node + gen collisions once to be able to check collision state outside of WorldSystem::handle_collisions
					if (!registry.collisions.has(entity_i) && !registry.collisions.has(player))
					{
						registry.collisions.emplace(entity_i, player);
						registry.collisions.emplace(player, entity_i);
					}
				}
				else
				{
					// Remove node + gen collisions once they stop colliding
					if (registry.collisions.has(entity_i) && registry.collisions.has(player))
					{
						registry.collisions.clear();
					}
				}
			}

		}
	}
}

void OrganPhysics::playerMovementHandlerOrgans(foregroundMotion& player_motion, float elapsed_ms) {
	float step_seconds = elapsed_ms / 1000.f;
	float new_pos = 0;
	int sprite_offset = 30;
	for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
		if (keymap[i]) {
			switch (i) {
			case GLFW_KEY_W:
				new_pos = player_motion.position.y - (275 * step_seconds);
				if (!checkOrganWallY({ player_motion.position.x, new_pos - 50 }, sprite_offset))
					player_motion.position.y += -250 * step_seconds;
				break;
			case GLFW_KEY_S:
				new_pos = player_motion.position.y + (275 * step_seconds);
				if (!checkOrganWallY({ player_motion.position.x, new_pos + 50 }, sprite_offset))
					player_motion.position.y += 250 * step_seconds;
				break;
			case GLFW_KEY_A:
				new_pos = player_motion.position.x - (275 * step_seconds);
				if (!checkOrganWallX({ new_pos - 50, player_motion.position.y }, sprite_offset))
					player_motion.position.x += -250 * step_seconds;
				break;
			case GLFW_KEY_D:
				new_pos = player_motion.position.x + (275 * step_seconds);
				if (!checkOrganWallX({ new_pos + 50, player_motion.position.y }, sprite_offset))
					player_motion.position.x += 250 * step_seconds;
				break;
			}
		}
		// Bounds the player in the game screen
		player_motion.position.x = max(50.f, min((float)window_width_px - 50, player_motion.position.x));
		player_motion.position.y = max(50.f, min((float)window_height_px - 50, player_motion.position.y));
	}
}


// Checks below are used to check for Organ wall collision. Divided by 25 since grid is split into 25x25 pixel squares
bool OrganPhysics::checkOrganWallX(vec2 pos, int offset) {
	int xCoord = (int)pos.x / 25;
	int(*boundaryPointer)[64];

	switch (game_state) {
	case (int)GAME_STATES::ORGAN_1:
		boundaryPointer = ORGAN_1_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_2:
		boundaryPointer = ORGAN_2_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_3:
		boundaryPointer = ORGAN_3_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_4:
		boundaryPointer = ORGAN_4_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_5:
		boundaryPointer = ORGAN_5_BOUNDARY;
		break;
	case (int)GAME_STATES::BRAIN_LOCKED:
		boundaryPointer = BRAIN_LOCKED_BOUNDARY;
		break;
	case (int)GAME_STATES::BRAIN_UNLOCKED:
		boundaryPointer = BRAIN_UNLOCKED_BOUNDARY;
		break;
	}
	
	return boundaryPointer[(int)(pos.y - offset) / 25][xCoord] || boundaryPointer[(int)(pos.y + offset) / 25][xCoord];
}

bool OrganPhysics::checkOrganWallY(vec2 pos, int offset) {
	int yCoord = (int)pos.y / 25;
	int(*boundaryPointer)[64];

	switch (game_state) {
	case (int)GAME_STATES::ORGAN_1:
		boundaryPointer = ORGAN_1_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_2:
		boundaryPointer = ORGAN_2_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_3:
		boundaryPointer = ORGAN_3_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_4:
		boundaryPointer = ORGAN_4_BOUNDARY;
		break;
	case (int)GAME_STATES::ORGAN_5:
		boundaryPointer = ORGAN_5_BOUNDARY;
		break;
	case (int)GAME_STATES::BRAIN_LOCKED:
		boundaryPointer = BRAIN_LOCKED_BOUNDARY;
		break;
	case (int)GAME_STATES::BRAIN_UNLOCKED:
		boundaryPointer = BRAIN_UNLOCKED_BOUNDARY;
		break;
	}

	return boundaryPointer[yCoord][(int)(pos.x - offset) / 25] || boundaryPointer[yCoord][(int)(pos.x + offset) / 25];
}