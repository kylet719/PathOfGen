// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms) {
	if (game_state == (unsigned int)GAME_STATES::TITLE) return;
	CommonAI* ai = getAI();
	ai->step(elapsed_ms);
}

CommonAI* AISystem::getAI() {
	switch (game_state) {
	case (unsigned int)GAME_STATES::MINIGAME_1: {
		return mg1AI;
	}
	case (unsigned int)GAME_STATES::MINIGAME_2: {
		return mg2AI;
	}
	case (unsigned int)GAME_STATES::MINIGAME_3: {}
	case (unsigned int)GAME_STATES::MINIGAME_4: {}
	case (unsigned int)GAME_STATES::MINIGAME_5: {}
	case (unsigned int)GAME_STATES::ORGAN_1: {}
	case (unsigned int)GAME_STATES::ORGAN_2: {}
	case (unsigned int)GAME_STATES::ORGAN_3: {}
	case (unsigned int)GAME_STATES::ORGAN_4: {}
	case (unsigned int)GAME_STATES::ORGAN_5: {}
	case (unsigned int)GAME_STATES::BRAIN_LOCKED: {}
	case (unsigned int)GAME_STATES::BRAIN_UNLOCKED: {}
	case (unsigned int)GAME_STATES::BRAIN_KILL: {}
	case (unsigned int)GAME_STATES::BRAIN_HELP: {}
	case (unsigned int)GAME_STATES::CREDITS: {

		return dummyAI;
	}
	default: {
		assert(false && "Need to implement an AI system for new game states");
		// Compiler doesn't like if this path doesn't return anything
		return dummyAI;
	}
	}
}

