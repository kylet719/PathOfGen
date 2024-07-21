// internal
#include "physics_system.hpp"

void PhysicsSystem::step(float elapsed_ms){
	CommonPhysics* physics = getPhysics();

	// if we are in tutorial state we want all physics to pause
	if (pause_game_state) return;

	physics->step(elapsed_ms);
}

CommonPhysics* PhysicsSystem::getPhysics() {
	switch (game_state) {
		case (unsigned int) GAME_STATES::TITLE: {
			return titlePhysics;
		}
		case (unsigned int) GAME_STATES::MINIGAME_1: {
			return mg1Physics;
		}
		case (unsigned int)GAME_STATES::MINIGAME_2: {
			return mg2Physics;
		}
		case (unsigned int)GAME_STATES::MINIGAME_3: {
			return mg3Physics;
		}
		case (unsigned int)GAME_STATES::MINIGAME_4: {
			return mg4Physics;
		}
		case (unsigned int)GAME_STATES::MINIGAME_5: {
			return mg5Physics;
		}
		case (unsigned int)GAME_STATES::CREDITS: {
			return creditsPhysics;
		}
		case (unsigned int)GAME_STATES::ORGAN_1: {}
		case (unsigned int)GAME_STATES::ORGAN_2: {}
		case (unsigned int)GAME_STATES::ORGAN_3: {}
		case (unsigned int)GAME_STATES::ORGAN_4: {}
		case (unsigned int)GAME_STATES::ORGAN_5: {}
		case (unsigned int)GAME_STATES::BRAIN_LOCKED: {}
		case (unsigned int)GAME_STATES::BRAIN_UNLOCKED: {}
		case (unsigned int)GAME_STATES::BRAIN_KILL: {}
		case (unsigned int)GAME_STATES::BRAIN_HELP: {
			return organPhysics;
		}

		default: {
			assert(false && "Need to implement a physics system for new game states");
			// Compiler doesn't like if this path doesn't return anything
			return titlePhysics;
		}
	}
}

