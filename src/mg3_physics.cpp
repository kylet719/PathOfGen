#include "mg3_physics.hpp"
#include <iostream>
#include <cstdlib>
#include <components.hpp>
#include <SDL_mixer.h>
#include "world_init.hpp"

bool on_platform = false;
float gravity_factor = 82.f;
int jump_count = 0;
bool is_in_acid(Entity& entity);

void MiniGame3Physics::step(float elapsed_ms) 
{
	float step = elapsed_ms / 1000.0f;

	auto& foregroundMotions_registry = registry.foregroundMotions;
	for (Entity platform_entity : registry.platform.entities)
	{
		foregroundMotion& motion = foregroundMotions_registry.get(platform_entity);
		motion.position += motion.velocity * step;
	}

	for (Entity finish_line : registry.finishLine.entities)
	{
		foregroundMotion& motion = foregroundMotions_registry.get(finish_line);
		motion.position += motion.velocity * step;
	}

	Entity& player = registry.players.entities[0];
	foregroundMotion& player_motion = registry.foregroundMotions.get(player);

	glucose_movement(elapsed_ms);
	handlePlayerMovement(elapsed_ms, player, player_motion);
	handlePlatformCollisions(player, player_motion);
	checkOffPlatform(player, player_motion);
	handleOffPlatformSideCollisions(player, player_motion);
	collisionDetection();
}

void MiniGame3Physics::handlePlayerMovement(float elapsed_ms, Entity entity, foregroundMotion& player_motion)
{
	float step = elapsed_ms / 1000.f;
	float jumping_step = elapsed_ms / 100.f;

	player_motion.velocity.x = 0;
	// Free Left / Right movement 
	for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
		if (keymap[i]) {
			switch (i) {
			case GLFW_KEY_A:
				if (!disable_a)
					player_motion.velocity.x = -pow(POS_STEP * 2.5, 2);
				break;
			case GLFW_KEY_D:
				if (!disable_d)
					player_motion.velocity.x = pow(POS_STEP * 2.5, 2);
				break;
			}
		}
	}

	player_motion.position.x += player_motion.velocity.x * step;
	// Player jumping movement
	if (registry.jump.has(entity))
	{
		player_motion.velocity.y += (gravity_factor * step * 6);

		if (player_motion.velocity.y >= gravity_factor)
		{
			player_motion.velocity.y = gravity_factor * 3;
			registry.jump.remove(entity);
		}

		player_motion.position.y += player_motion.velocity.y * step * 2;
	}
	else
	{
		if (on_platform)
			player_motion.velocity.y = gravity_factor;
		else
			player_motion.velocity.y = gravity_factor * 3;

		player_motion.position.y += player_motion.velocity.y * step;
	}

	// Bounds the player in the game screen
	player_motion.position.x = max(50.f, min((float)window_width_px - 50, player_motion.position.x));
	player_motion.position.y = max(50.f, min((float)window_height_px - 50, player_motion.position.y));
}

void MiniGame3Physics::glucose_movement(float elapsed_ms)
{
	float step = elapsed_ms / 1000.f;

	auto& consumable_registry = registry.consumables;
	for (Entity entity : consumable_registry.entities)
	{
		foregroundMotion& g_motion = registry.foregroundMotions.get(entity);

		if (g_motion.velocity.y < gravity_factor * 3)
		{
			g_motion.velocity.y += (gravity_factor * step * 3);

			if (g_motion.velocity.y > gravity_factor * 3)
			{
				g_motion.velocity.y = gravity_factor * 3;
			}
			g_motion.position.y += g_motion.velocity.y * step * 2;
		}
		else
		{
			g_motion.position.y += g_motion.velocity.y * step;
			if (g_motion.velocity.x != 0)
			{
				g_motion.velocity.x -= gravity_factor * step;

				if (g_motion.velocity.x < 0)
				{
					g_motion.velocity.x = 0;
				}
			}
		}
		
		g_motion.position.x += g_motion.velocity.x * step;

		// Bounds glucose to the sides of the game screen
		g_motion.position.x = max(50.f, min((float)window_width_px - 50, g_motion.position.x));
		g_motion.angle += float(std::atan2(g_motion.velocity.x, g_motion.velocity.y) * step * 1.5);

		if (g_motion.position.x <= 50 || g_motion.position.x >= (float)window_width_px - 50)
			g_motion.velocity.x *= -1;
	}
}

void MiniGame3Physics::checkOffPlatform(Entity& entity, foregroundMotion& player_motion)
{
	if (on_platform)
	{
		vec2 player_bb = get_bounding_box(entity);
		vec4 player_bbox_corners = get_bbox_corners(player_bb, player_motion.position);

		float x1_min = player_bbox_corners[0];
		float x1_max = player_bbox_corners[1];

		if (registry.foregroundMotions.has(player_on_platform))
		{
			foregroundMotion& platform_motion = registry.foregroundMotions.get(player_on_platform);
			vec2 platform_bb = get_bounding_box(player_on_platform);
			vec4 platform_bbox_corners = get_bbox_corners(platform_bb, platform_motion.position);

			float x2_min = platform_bbox_corners[0];
			float x2_max = platform_bbox_corners[1];

			if (x1_min > x2_min && x1_min > (x2_max - 5))
			{
				Platform& platform = registry.platform.get(player_on_platform);
				platform.below = false;
				on_platform = false;
				player_motion.position.x = x2_max + (player_bb.x / 2);
			}
			else if (x1_max < (x2_min + 5) && x1_max < x2_max)
			{
				Platform& platform = registry.platform.get(player_on_platform);
				platform.below = false;
				on_platform = false;
				player_motion.position.x = x2_min - (player_bb.x / 2);
			}
		}
		else
		{
			on_platform = false;
		}
	}
}

void MiniGame3Physics::handlePlatformCollisions(Entity& player, foregroundMotion& player_motion)
{
	vec2 player_position = player_motion.position;
	vec2 player_bb = get_bounding_box(player);
	vec4 player_bbox_corners = get_bbox_corners(player_bb, player_position);

	float x1_min = player_bbox_corners[0];
	float x1_max = player_bbox_corners[1];
	float y1_min = player_bbox_corners[2];
	float y1_max = player_bbox_corners[3];

	// Platform & Player Collisions
	for (Entity platform_entity : registry.platform.entities)
	{ 
		foregroundMotion& platform_motion = registry.foregroundMotions.get(platform_entity);
		vec2 platform_position = platform_motion.position;
		vec2 platform_bb = get_bounding_box(platform_entity);
		vec4 platform_bbox_corners = get_bbox_corners(platform_bb, platform_position);

		float x2_min = platform_bbox_corners[0];
		float x2_max = platform_bbox_corners[1];
		float y2_min = platform_bbox_corners[2];
		float y2_max = platform_bbox_corners[3];

		// only check for platform landings after jumping
		if (!on_platform)
		{
			// Only land on platforms that are below the player
			Platform& platform = registry.platform.get(platform_entity);
			if (platform.below)
			{
				if ((y1_max >= y2_min && y1_min < y2_min) && ((x1_min > x2_min && (x1_min < (x2_max - 20))) || ((x1_max > (x2_min + 20)) && x1_max < x2_max)))
				{
					on_platform = true;
					player_on_platform = platform_entity;
					jump_count = 0;
				}
				else
					on_platform = false;
			}

			// update if platforms are lower than player
			if (y2_min >= y1_max && y2_min > y1_min)
				platform.below = true;
			else
				platform.below = false;

		}

		auto& consumable_registry = registry.consumables;
		for (Entity consumable : consumable_registry.entities)
		{
			foregroundMotion& c_motion = registry.foregroundMotions.get(consumable);
			vec2 glucose_position = c_motion.position;
			vec2 glucose_bb = get_bounding_box(consumable);
			vec4 glucose_bbox_corners = get_bbox_corners(glucose_bb, glucose_position);

			float x3_min = glucose_bbox_corners[0];
			float x3_max = glucose_bbox_corners[1];
			float y3_min = glucose_bbox_corners[2];
			float y3_max = glucose_bbox_corners[3];

			if ((y3_max >= y2_min && y3_min < y2_min) && ((x3_min > x2_min && (x3_min < (x2_max - 20))) || ((x3_max > (x2_min + 20)) && x3_max < x2_max)))
			{
				if (c_motion.velocity.x == 0)
				{
					c_motion.velocity.x = 50 * random_dir;
					random_dir *= -1;
				}

				c_motion.velocity.y = -100;
			}		
		}
	}

}

void MiniGame3Physics::collisionDetection()
{
	// Acid Collisions
	ComponentContainer<Collidable>& collidableContainer = registry.collidables;
	for (uint i = 0; i < collidableContainer.entities.size(); i++)
	{
		Entity& entity_i = collidableContainer.entities[i];

		for (uint j = 0; j < collidableContainer.entities.size(); j++)
		{
			Entity& entity_j = collidableContainer.entities[j];

			bool oneIsAcid = registry.deadlys.has(entity_i) || registry.deadlys.has(entity_j);
			bool oneIsPlayer = registry.players.has(entity_i) || registry.players.has(entity_j);
			bool oneIsPlatform = registry.platform.has(entity_i) || registry.platform.has(entity_j);
			bool oneIsGlucose = registry.consumables.has(entity_i) || registry.consumables.has(entity_j);
			bool oneIsFinishLine = registry.finishLine.has(entity_i) || registry.finishLine.has(entity_j);

			if (oneIsAcid && oneIsPlatform && (is_in_acid(entity_i) && is_in_acid(entity_j)))
			{
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
			else if (oneIsAcid && oneIsPlayer && (is_in_acid(entity_i) && is_in_acid(entity_j)))
			{
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
			else if (oneIsAcid && oneIsGlucose && (is_in_acid(entity_i) && is_in_acid(entity_j)))
			{
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
			else if (oneIsPlayer && oneIsGlucose && checkBoxCollision(entity_i, entity_j))
			{
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
			else if (oneIsPlayer && oneIsFinishLine && playerFinish(entity_i, entity_j))
			{
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}
}

void MiniGame3Physics::handleOffPlatformSideCollisions(Entity& player, foregroundMotion& player_motion)
{
	vec2 player_position = player_motion.position;
	vec2 player_bb = get_bounding_box(player);
	vec4 player_bbox_corners = get_bbox_corners(player_bb, player_position);

	float x1_min = player_bbox_corners[0];
	float x1_max = player_bbox_corners[1];
	float y1_min = player_bbox_corners[2];
	float y1_max = player_bbox_corners[3];

	// Platform & Player Collisions
	for (Entity platform_entity : registry.platform.entities)
	{
		foregroundMotion& platform_motion = registry.foregroundMotions.get(platform_entity);
		vec2 platform_position = platform_motion.position;
		vec2 platform_bb = get_bounding_box(platform_entity);
		vec4 platform_bbox_corners = get_bbox_corners(platform_bb, platform_position);

		float x2_min = platform_bbox_corners[0];
		float x2_max = platform_bbox_corners[1];
		float y2_min = platform_bbox_corners[2];
		float y2_max = platform_bbox_corners[3];

		// check for off-platform collisions with other platforms
		if (!on_platform)
		{
			// check for player right side collisions to left side of platform
			if (checkBoxCollision(player, platform_entity))
			{
				// collisions with bottom of the platform
				if (y1_min < y2_max && y1_min > y2_min && (x1_max > (x2_min + 20) && x1_max < (x2_max - 20) || x1_min > (x2_min + 20) && x1_min < (x2_max - 20)))
				{
					player_motion.position.y = y2_max + (player_bb.y / 2);
					player_motion.velocity.y = gravity_factor * 3;
					break;
				}

				// side collision
				if (x1_max > x2_max)
				{
					disable_a = true;
					break;
				}
				else
				{
					disable_d = true;
					break;
				}

				
			}
			else
			{
				disable_a = false;
				disable_d = false;
			}
		}
	}
}

bool MiniGame3Physics::playerFinish(Entity& entity_i, Entity& entity_j)
{
	if (registry.players.has(entity_i) && registry.finishLine.has(entity_j))
	{
		foregroundMotion& player_motion = registry.foregroundMotions.get(entity_i);
		foregroundMotion& finish_line_motion = registry.foregroundMotions.get(entity_j);

		float player_y = player_motion.position.y;
		float finish_line_y = finish_line_motion.position.y;
		vec2 player_bb = get_bounding_box(entity_i);
		vec2 finish_line_bb = get_bounding_box(entity_j);

		return player_y + (player_bb.y / 2) <= finish_line_y - (player_bb.y / 2);
	}
	return false;
}

bool is_in_acid(Entity& entity)
{
	Entity& acid = registry.deadlys.entities[0];
	foregroundMotion& acid_motion = registry.foregroundMotions.get(acid);
	foregroundMotion& motion = registry.foregroundMotions.get(entity);
	
	return acid_motion.position.y <= motion.position.y;
}
