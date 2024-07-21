// internal
#include "mg2_ai.hpp"
#include "world_init.hpp"
#include "random"

float TOTAL_TIME = 0.0f;
float tickTimer = 0.f;

// C++ random number generator
std::default_random_engine rng;
std::uniform_real_distribution<float> uniform_dist; // number between 0..1

// Once this amount of timee has passed, activate hard mode
float HARD_MODE_TIME = 5000.0f;

AITreeNode root;
AITreeNode one;
AITreeNode two;
AITreeNode three;
AITreeNode four;
AITreeNode five;

RenderSystem* render;

void MiniGame2AI::step(float elapsed_ms)
{
	if (pause_game_state == true) return;
	TOTAL_TIME += elapsed_ms;
	tickTimer += elapsed_ms;
	
	if (tickTimer > 4166.f) {
		processNode(&root);
		tickTimer = 0;
	}
	TICK++;
}

// Initializes behavior tree for MG2. Only three nodes right now, (root + left + right) but open to extension.
void initializeMG2Tree(RenderSystem* renderer) {
	TOTAL_TIME = 0.0f;
	render = renderer;

	three.isEnd = true;
	// leaf node, generate worm on top of the screen if Gen y position is less than half the screen
	three.Action = []() {
		createMG2MeshObject(render, { (window_width_px + 100), 100 }, { 150,-150 }, 1, 0);
		};

	two.isEnd = true;
	// leaf node, generate worm on bottom of the screen if Gen y position is more or equal to half the screen
	two.Action = []() {
		createMG2MeshObject(render, { (window_width_px + 100), (window_height_px - 100) }, { 75,75 }, 0, 0);
		};

	one.isEnd = false;
	// parent node, generate worms either on top or bottom of screen based on Gen's positions
	one.left = &two;
	one.right = &three;
	one.Decision = []() -> bool {
		Entity& player = registry.players.entities[0];
		foregroundMotion& player_motion = registry.foregroundMotions.get(player);
		return player_motion.position.y >= window_height_px / 2.0f;
		};
	
	four.isEnd = true;
	// leaf node, HARD MODE
	four.Action = []() {
		createMG2MeshObject(render, { (window_width_px + 100), (window_height_px - 100) }, { 75,75 }, 0, uniform_dist(rng));
		createMG2MeshObject(render, { (window_width_px + 400), 100 }, { 150,-150 }, 1, uniform_dist(rng));
		createMG2MeshObject(render, { (window_width_px + 100) + 2 * window_width_px, window_height_px / 1.5 }, { 75,75 }, 2, uniform_dist(rng));
		};

	five.isEnd = false;
	five.left = &four;
	five.right = &one;
	// If enough time has passed, activate hard mode.
	five.Decision = []() -> bool {
		return TOTAL_TIME >= HARD_MODE_TIME;
		};

	root = five;

}