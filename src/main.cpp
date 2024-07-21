
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "ai_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point 
int main(int argc, char* argv[])
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	srand((unsigned)time(NULL));
	renderer.init(window);
	world.init(&renderer);

	// variable timestep loop
	auto t = Clock::now();
	const float FRAME_CAP = 60.0f;
	const float FRAME_TIME = 1000.0f / FRAME_CAP;
	float accumulator = 0.0f;

	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		accumulator += elapsed_ms;

		// do one step of the world, ai ,and physics once accumulator > 16.67ms. 
		// step each with constant 16.67 ms since last update.
		while (accumulator >= FRAME_TIME) {
			world.step(elapsed_ms); // Step the whole world (so like game environment and screen)
			ai.step(elapsed_ms);
			physics.step(elapsed_ms); // Step the physics system (so move characters given inputs)
			world.handle_collisions(); // After moving, check for collisions and process those

			accumulator -= elapsed_ms;
		}

		renderer.draw(); // Finally, draw new updated state to the screen
	}

	return EXIT_SUCCESS;
}
