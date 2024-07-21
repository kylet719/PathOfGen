#include "title_physics.hpp"

// BACKGROUND ENTITIES
// 0: background
// 1: Title splash art
// 2: Selection arrow
// 3: Options to start/load save
// 4: Selection controls

const float SPLASH_ART_Y_POS = 300;
const float TITLE_ARROW_SELECT_START_Y_POS = 690;
const float TITLE_ARROW_SELECT_LOAD_Y_POS = 755;
const float TITLE_ARROW_SELECT_CREDITS_Y_POS = 820;
const float TITLE_OPTIONS_Y_POS = 750;
const float TITLE_CONTROLS_Y_POS = window_height_px - 60;

float arrow_pos = TITLE_ARROW_SELECT_START_Y_POS;

void TitlePhysics::step(float elapsed_ms) {
	float step_seconds = elapsed_ms / 1000.f;
	if (panUp) {
		screenMoves_panUpwards(step_seconds);
		title_pans_down(step_seconds);
	}
	else {
		screenMoves_panDown(step_seconds);
	}

	if (registry.title.components[0].titleInPlace) {
		selectionKeyHandler();
		registry.backgroundMotions.get(registry.background.entities[1]).position.y = SPLASH_ART_Y_POS;
		registry.backgroundMotions.get(registry.background.entities[2]).position.y = arrow_pos;
		registry.backgroundMotions.get(registry.background.entities[3]).position.y = TITLE_OPTIONS_Y_POS;
		registry.backgroundMotions.get(registry.background.entities[4]).position.y = TITLE_CONTROLS_Y_POS;
	}
}


void TitlePhysics::screenMoves_panUpwards(float step_seconds) {
	std::vector<Entity>& backgroundRenderEntities = registry.backgroundRenderRequests.entities;
	Entity& backgroundEntity = registry.background.entities[0]; // 0th element is big background
	if (registry.backgroundMotions.get(backgroundEntity).position.y <= window_height_px+1200) {
		registry.backgroundMotions.get(backgroundEntity).position.y += step_seconds * 75.f;
	}
	else {
		panUp = false;
	}
}

void TitlePhysics::screenMoves_panDown(float step_seconds) {
	std::vector<Entity>& backgroundRenderEntities = registry.backgroundRenderRequests.entities;
	Entity& backgroundEntity = registry.background.entities[0]; // 0th element is big background
	if (registry.backgroundMotions.get(backgroundEntity).position.y >= -850) { 
		registry.backgroundMotions.get(backgroundEntity).position.y -= step_seconds * 75.f;
	}
	else {
		panUp = true;
	}
	
}

void TitlePhysics::title_pans_down(float step_seconds) {
	std::vector<Entity>& backgroundRenderEntities = registry.backgroundRenderRequests.entities;
	Entity& titleEntity = registry.background.entities[1]; // one'th element is the title
	if (registry.backgroundMotions.get(titleEntity).position.y <= SPLASH_ART_Y_POS) {
		registry.backgroundMotions.get(titleEntity).position.y += step_seconds * 200.f;

	}
	else {
		registry.title.components[0].titleInPlace = true;
	}
}

void TitlePhysics::selectionKeyHandler() {
	if (registry.title.components[0].selectionOption == 0) {
		arrow_pos = TITLE_ARROW_SELECT_START_Y_POS;
	}
	else if (registry.title.components[0].selectionOption == 1) {
		arrow_pos = TITLE_ARROW_SELECT_LOAD_Y_POS;
	}
	else {
		arrow_pos = TITLE_ARROW_SELECT_CREDITS_Y_POS;
	}
}
