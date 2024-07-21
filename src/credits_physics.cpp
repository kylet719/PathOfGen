#include "credits_physics.hpp"
#include "world_init.hpp"

// Gen Animation
bool animationOneStarted = false;
bool animationOneFinished = false;
Entity gen;

// Red blood cell animation
bool animationTwoStarted = false;
bool animationTwoFinished = false;
Entity redBloodEnemy;

// Acid platform animation
bool animationThreeStarted = false;
bool animationThreeFinished = false;
bool acidInPlace = false;
bool platformsFinished = false;
Entity acid;
std::vector<Entity> platforms;

// Brick breaker animation
bool animationFourStarted = false;
bool animationFourFinished = false;
std::vector<Entity> bricks;
Entity ball;
int direction = 1;
unsigned int brickPointer = 0;
bool bricksInPlace = false;

// Items mixing animations
bool animationFiveStarted = false;
bool animationFiveFinished = false;
Entity oxygen;
const vec2 OXYGEN_STARTING_POS = {-150, -150};
const vec2 OXYGEN_STARTING_SCALE = { 100, -100 };
Entity atp;
const vec2 ATP_STARTING_POS = {window_width_px + 150, -150};
const vec2 ATP_STARTING_SCALE = {100, -100};
Entity glucose;
const vec2 GLUCOSE_STARTING_POS = { window_width_px + 150, window_height_px + 150 };
const vec2 GLUCOSE_STARTING_SCALE = { 100, -100 };
Entity iron;
const vec2 IRON_STARTING_POS = {-150, window_height_px + 150 };
const vec2 IRON_STARTING_SCALE = { 100, -100 };
Entity lipid;
const vec2 LIPID_STARTING_POS = { window_width_px/2, -150};
const vec2 LIPID_STARTING_SCALE = { 100, -100 };
Entity pill;
const vec2 PILL_ENDING_SCALE = { 100, -100 };
const vec2 PILL_ENDING_POS = { window_width_px - 300, -150 };
Entity poison;
const vec2 POISON_ENDING_SCALE = { 100, -100 };
const vec2 POISON_ENDING_POS = { window_width_px - 300, window_height_px +150};
const vec2 ITEM_MIX_POS = { window_width_px -300,  window_height_px/2 };
float accuTimeItemAnimation = 0;

// Humans popping into frame
bool animationSixStarted = false;
bool animationSixFinished = false;
Entity woman;
const vec2 WOMAN_ENDING_POS = { -50, window_height_px / 2 };
Entity man;
const vec2 MAN_ENDING_POS = { window_width_px + 50, window_height_px / 2 };
float accuTimeHumanAnimation = 0;

// Tiny Gen in corner
bool animationSevenStarted = false;
bool animationSevenFinished = false;
Entity tinyGen;
const vec2 TINY_GEN_ENDING_POS = { window_width_px-1, window_height_px - 65 };
float accuTimeTinyGenAnimation = 0;

void CreditsPhysics::step(float elapsed_ms) {
	CreditsPhysics::screenMoves_panDown(elapsed_ms);
	if (!registry.credits.components[0].creditsStarted) {
		// If not started, init
		animationOneStarted = animationOneFinished = false;
		animationTwoStarted = animationTwoFinished = false;
		animationThreeStarted = animationThreeFinished = false;
		animationFourStarted = animationFourFinished = false;
		animationFiveStarted = animationFiveFinished = false;
		animationSixStarted = animationSixFinished = false;
		animationSevenStarted = animationSevenFinished = false;
		acidInPlace = platformsFinished =false;
		direction = 1;
		brickPointer = 0;
		bricksInPlace = false;
		accuTimeItemAnimation = 0;
		accuTimeHumanAnimation = 0;
		accuTimeTinyGenAnimation = 0;
		registry.credits.components[0].creditsStarted = true;
	}
	CreditsPhysics::moveAnimation(elapsed_ms);
}


void CreditsPhysics::screenMoves_panDown(float elapsed_ms) {
	std::vector<Entity>& backgroundRenderEntities = registry.backgroundRenderRequests.entities;
	Entity& backgroundEntity = registry.background.entities[0]; // 0th element is big background
	if (registry.backgroundMotions.get(backgroundEntity).position.y > -2800) {
		registry.backgroundMotions.get(backgroundEntity).position.y -= 0.066 * elapsed_ms;
		// Gen rolling across bottom
		if (registry.backgroundMotions.get(backgroundEntity).position.y < 3000 && !animationOneStarted && !animationOneFinished) {
			gen = createGenericTexture({ window_width_px + 200, window_height_px - 75 }, { 150, 150 }, TEXTURE_ASSET_ID::GEN);
			animationOneStarted = true;
		}
		// Red blood blobby enemy 
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < 2300 && !animationTwoStarted && !animationTwoFinished) {
			redBloodEnemy = createRedBloodCell({ window_width_px / 2, window_height_px / 2 });
			registry.foregroundMotions.get(redBloodEnemy).position = { 22, -50 };
			registry.foregroundMotions.get(redBloodEnemy).scale.x *= -1;
			registry.foregroundMotions.get(redBloodEnemy).angle = -M_PI/2;
			animationTwoStarted = true;
		}
		// Acid and platforms falling
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < 1550 && !animationThreeStarted && !animationThreeFinished) {
			acid = createAcid({ window_width_px / 2, window_height_px + 100});
			Entity p1 = createPlatform({ 130, -100 - 500}, { 250, -100 }, 0);
			platforms.push_back(p1);
			Entity p2 = createPlatform({ 800, -600 - 500}, { 250, -100 }, 0);
			platforms.push_back(p2);
			Entity p3 = createPlatform({ 1100, -300 - 500}, { 250, -100 }, 0);
			platforms.push_back(p3);
			Entity p4 = createPlatform({ 480, -900 - 500}, { 250, -100 }, 0);
			platforms.push_back(p4);
			Entity p5 = createPlatform({ 1300, -700 - 500}, { 250, -100 }, 0);
			platforms.push_back(p5);
			animationThreeStarted = true;
		}
		// Brick breaker
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < 0 && !animationFourStarted && !animationFourFinished) {
			for (unsigned int i = 0; i < 17; i++) {
				Entity b1 = createBrick({ (float)i * 100-window_width_px, window_height_px -100}, { 100, 50 }, false, PowerUpType::NONE);
				bricks.emplace_back(b1);
				Entity b2 = createBrick({ (float)i * 100+50- window_width_px, window_height_px - 300}, { 100, 50 }, false, PowerUpType::NONE);
				bricks.emplace_back(b2);
			}
			ball = createBall({ window_width_px / 2, window_height_px - 200.0f }, { 0.f, -300.f });
			registry.foregroundMotions.get(ball).position.x = -100;
			
			animationFourStarted = true;
		}
		// Items popping into the corner of the screens
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < -900 && !animationFiveStarted && !animationFiveFinished) {
			atp = createGenericTexture(ATP_STARTING_POS, ATP_STARTING_SCALE, TEXTURE_ASSET_ID::ITEM_ATP);
			lipid = createGenericTexture(LIPID_STARTING_POS, LIPID_STARTING_SCALE, TEXTURE_ASSET_ID::ITEM_LIPID);
			iron = createGenericTexture(IRON_STARTING_POS, IRON_STARTING_SCALE, TEXTURE_ASSET_ID::IRON);
			oxygen = createGenericTexture(OXYGEN_STARTING_POS, OXYGEN_STARTING_SCALE, TEXTURE_ASSET_ID::OXYGEN);
			glucose = createGenericTexture(GLUCOSE_STARTING_POS, GLUCOSE_STARTING_SCALE, TEXTURE_ASSET_ID::ITEM_GLUCOSE);
			pill = createGenericTexture(ITEM_MIX_POS, vec2({0, 0}), TEXTURE_ASSET_ID::CREDITS_PILL);
			poison = createGenericTexture(ITEM_MIX_POS, vec2({ 0, 0 }), TEXTURE_ASSET_ID::CREDITS_POISON);
			animationFiveStarted = true;
		}
		// Humans popping up on sides
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < -1600 && !animationSixStarted && !animationSixFinished) {
			woman = createGenericTexture({-250, window_height_px/2}, { 400, -486 }, TEXTURE_ASSET_ID::CREDITS_ZOMBIE_MAN);
			man = createGenericTexture({window_width_px+250, window_height_px/2}, { 400, -420 }, TEXTURE_ASSET_ID::CREDITS_MAN);
			animationSixStarted = true;
		}
		// Tiny Gen in the corner
		else if (registry.backgroundMotions.get(backgroundEntity).position.y < -2750 && !animationSevenStarted && !animationSevenFinished) {
			tinyGen = createBall({ window_width_px / 2, window_height_px - 200.0f }, { 0.f, -300.f });
			registry.foregroundMotions.get(tinyGen).position = TINY_GEN_ENDING_POS;
			animationSevenStarted = true;
		}

	}
	else {
		registry.credits.components[0].creditsFinished = true;
	} 
	
}

void CreditsPhysics::moveAnimation(float elapsed_ms) {
	if (animationOneStarted) {
		// Rotate and move Gen
		if (registry.foregroundMotions.get(gen).position.x < -100) {
			animationOneStarted = false;
			animationOneFinished = true;
			registry.remove_all_components_of(gen);
		}
		else {
			registry.foregroundMotions.get(gen).position.x -= 0.18 * elapsed_ms;
			registry.foregroundMotions.get(gen).angle -= 0.0015 * elapsed_ms;
		}
	}
	if (animationTwoStarted) {
		// Red blood cell inching across screen
		if (registry.foregroundMotions.get(redBloodEnemy).position.y > window_height_px+100) {
			animationTwoStarted = false;
			animationTwoFinished = true;
			registry.remove_all_components_of(redBloodEnemy);
		}
		else {
			registry.foregroundMotions.get(redBloodEnemy).position.y += 0.08 * elapsed_ms;
		}
	}
	if (animationThreeStarted) {
		// Move acid into frame
		if (registry.foregroundMotions.get(acid).position.y < window_height_px-45 && !platformsFinished) {
			acidInPlace = true;
		}
		else if (!platformsFinished) {
			registry.foregroundMotions.get(acid).position.y -= 0.03 * elapsed_ms;
		}
		// Move platforms
		for (unsigned int i = 0; i < platforms.size(); i++) {
			registry.foregroundMotions.get(platforms[i]).position.y += 0.12 * elapsed_ms;
		}
		if (registry.foregroundMotions.get(platforms[platforms.size() - 1]).position.y > window_height_px + 100) {
			platformsFinished = true;
		}

		if (platformsFinished) {
			registry.foregroundMotions.get(acid).position.y += 0.03 * elapsed_ms;
		}

		if (platformsFinished && registry.foregroundMotions.get(acid).position.y > window_height_px + 100) {
			animationThreeFinished = true;
			animationThreeStarted = false;
			registry.remove_all_components_of(acid);
			for (unsigned int i = 0; i < platforms.size(); i++) {
				registry.remove_all_components_of(platforms[i]);
			}
			platforms.clear();
		}
	}
	if (animationFourStarted) {

		// Move bricks in place
		if (!bricksInPlace) {
			for (unsigned int i = 0; i < bricks.size(); i++) {
				registry.backgroundMotions.get(bricks[i]).position.x += 0.4 * elapsed_ms;
			}
			if (registry.backgroundMotions.get(bricks[bricks.size() - 1]).position.x > 16 * 100) {
				bricksInPlace = true;
				registry.foregroundMotions.get(ball).position.x = -50;
			}
		}

		 //Ball movement
		if (bricksInPlace) {
			if (registry.foregroundMotions.get(ball).position.y > window_height_px - 150.0f) {
				if (direction == 1) {
					direction = -1;
					if (brickPointer < bricks.size()) {
						registry.remove_all_components_of(bricks[brickPointer]);
						brickPointer++;
					}
				}
			} 
			if (registry.foregroundMotions.get(ball).position.y < window_height_px - 250.0f) {
				if (direction == -1) {
					direction = 1;
					if (brickPointer < bricks.size()) {
						registry.remove_all_components_of(bricks[brickPointer]);
						brickPointer++;
					}
				}
			}

			registry.foregroundMotions.get(ball).position.x += 0.23 * elapsed_ms;
			registry.foregroundMotions.get(ball).position.y += 0.5 * direction * elapsed_ms;

			if (registry.foregroundMotions.get(ball).position.x > window_width_px + 200) {
				animationFourFinished = true;
				animationFourStarted = false;
				registry.remove_all_components_of(ball);
				bricks.clear();
			}
		}
	}
	if (animationFiveStarted) {
		// Move from one to another
		const float totalTimeToMove = 3000;
		const float totalTimeToShrink = 3000;
		const float totalTimeToGrow = 3000;
		const float totalTimeToMoveCombined = 3000;
		accuTimeItemAnimation += elapsed_ms;
		// Linear interpolation... Don't mind the math...
		if (accuTimeItemAnimation < totalTimeToMove) {
			registry.foregroundMotions.get(oxygen).position = (OXYGEN_STARTING_POS) * (1 - (accuTimeItemAnimation / totalTimeToMove)) + ITEM_MIX_POS * (accuTimeItemAnimation / totalTimeToMove);
			registry.foregroundMotions.get(iron).position = (IRON_STARTING_POS) * (1 - (accuTimeItemAnimation / totalTimeToMove)) + ITEM_MIX_POS * (accuTimeItemAnimation / totalTimeToMove);
			registry.foregroundMotions.get(glucose).position = (GLUCOSE_STARTING_POS) * (1 - (accuTimeItemAnimation / totalTimeToMove)) + ITEM_MIX_POS * (accuTimeItemAnimation / totalTimeToMove);
			registry.foregroundMotions.get(atp).position = (ATP_STARTING_POS) * (1 - (accuTimeItemAnimation / totalTimeToMove)) + ITEM_MIX_POS * (accuTimeItemAnimation / totalTimeToMove);
			registry.foregroundMotions.get(lipid).position = (LIPID_STARTING_POS) * (1 - (accuTimeItemAnimation / totalTimeToMove)) + ITEM_MIX_POS * (accuTimeItemAnimation / totalTimeToMove);
		}
		if (accuTimeItemAnimation >= totalTimeToMove && accuTimeItemAnimation < totalTimeToShrink+ totalTimeToMove) {
			registry.foregroundMotions.get(oxygen).scale = (OXYGEN_STARTING_SCALE) * (1 - ((accuTimeItemAnimation- totalTimeToMove) / totalTimeToShrink)) + vec2({ 0,0 }) * ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink);
			registry.foregroundMotions.get(iron).scale = (IRON_STARTING_SCALE) * (1 - ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink)) + vec2({ 0,0 }) * ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink);
			registry.foregroundMotions.get(glucose).scale = (GLUCOSE_STARTING_SCALE) * (1 - ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink)) + vec2({ 0,0 }) * ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink);
			registry.foregroundMotions.get(atp).scale = (ATP_STARTING_SCALE) * (1 - ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink)) + vec2({ 0,0 }) * ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink);
			registry.foregroundMotions.get(lipid).scale = (LIPID_STARTING_SCALE) * (1 - ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink)) + vec2({ 0,0 }) * ((accuTimeItemAnimation - totalTimeToMove) / totalTimeToShrink);
		}
		if (accuTimeItemAnimation >= (totalTimeToMove + totalTimeToShrink) && accuTimeItemAnimation < totalTimeToShrink + totalTimeToMove + totalTimeToGrow) {
			registry.foregroundMotions.get(pill).scale = (vec2({0, 0,})) * (1 - ((accuTimeItemAnimation - totalTimeToMove- totalTimeToShrink) / totalTimeToGrow)) + PILL_ENDING_SCALE * ((accuTimeItemAnimation - totalTimeToMove- totalTimeToShrink) / totalTimeToGrow);
			registry.foregroundMotions.get(poison).scale = (vec2({ 0, 0, })) * (1 - ((accuTimeItemAnimation - totalTimeToMove- totalTimeToShrink) / totalTimeToGrow)) + POISON_ENDING_SCALE * ((accuTimeItemAnimation - totalTimeToMove- totalTimeToShrink) / totalTimeToGrow);
		}
		if (accuTimeItemAnimation >= (totalTimeToMove + totalTimeToShrink + totalTimeToGrow) && accuTimeItemAnimation < totalTimeToShrink + totalTimeToMove + totalTimeToGrow + totalTimeToMoveCombined) {
			registry.foregroundMotions.get(pill).position = ITEM_MIX_POS * (1 - ((accuTimeItemAnimation - totalTimeToMove - totalTimeToShrink - totalTimeToGrow) / totalTimeToMoveCombined)) + PILL_ENDING_POS * ((accuTimeItemAnimation - totalTimeToMove - totalTimeToShrink - totalTimeToGrow) / totalTimeToMoveCombined);
			registry.foregroundMotions.get(poison).position = ITEM_MIX_POS * (1 - ((accuTimeItemAnimation - totalTimeToMove - totalTimeToShrink - totalTimeToGrow) / totalTimeToMoveCombined)) + POISON_ENDING_POS * ((accuTimeItemAnimation - totalTimeToMove - totalTimeToShrink - totalTimeToGrow) / totalTimeToMoveCombined);
		}
		if (accuTimeItemAnimation >= (totalTimeToMove + totalTimeToShrink + totalTimeToGrow + totalTimeToMoveCombined)) {
			animationFiveFinished = true;
			animationFiveStarted = false;
			registry.remove_all_components_of(oxygen);
			registry.remove_all_components_of(iron);
			registry.remove_all_components_of(glucose);
			registry.remove_all_components_of(atp);
			registry.remove_all_components_of(lipid);
			registry.remove_all_components_of(pill);
			registry.remove_all_components_of(poison);
		}
	}
	if (animationSixStarted) {
		const float totalTimeToMove = 6000;
		const float totalWaitTime = 1300;
		
		accuTimeHumanAnimation += elapsed_ms;
		if (accuTimeHumanAnimation < totalTimeToMove) {
			registry.foregroundMotions.get(woman).position = vec2({ -250, window_height_px / 2 }) * (1 - (accuTimeHumanAnimation / totalTimeToMove)) + WOMAN_ENDING_POS * (accuTimeHumanAnimation / totalTimeToMove);
			registry.foregroundMotions.get(man).position = vec2({ window_width_px + 250, window_height_px / 2 }) * (1 - (accuTimeHumanAnimation / totalTimeToMove)) + MAN_ENDING_POS * (accuTimeHumanAnimation / totalTimeToMove);
		}
		if (accuTimeHumanAnimation >= totalTimeToMove && accuTimeHumanAnimation < totalWaitTime + totalTimeToMove) {
			// Do nothing and wait
		}
		if (accuTimeHumanAnimation >= (totalTimeToMove + totalWaitTime) && accuTimeHumanAnimation < totalWaitTime + totalTimeToMove+ totalTimeToMove) {
			registry.foregroundMotions.get(woman).position = WOMAN_ENDING_POS * (1 - ((accuTimeHumanAnimation - totalTimeToMove - totalWaitTime) / totalTimeToMove)) + vec2({ -250, window_height_px / 2 }) * ((accuTimeHumanAnimation - totalTimeToMove - totalWaitTime) / totalTimeToMove);
			registry.foregroundMotions.get(man).position = MAN_ENDING_POS * (1 - ((accuTimeHumanAnimation - totalTimeToMove - totalWaitTime) / totalTimeToMove)) + vec2({ window_width_px + 250, window_height_px / 2 }) * ((accuTimeHumanAnimation - totalTimeToMove - totalWaitTime) / totalTimeToMove);
		}
		if (accuTimeHumanAnimation >= (totalTimeToMove + totalWaitTime + totalTimeToMove)) {
			animationSixFinished = true;
			animationSixStarted = false;
			registry.remove_all_components_of(woman);
			registry.remove_all_components_of(man);
		}
	}
	if (animationSevenStarted) {
		const float totalTimeToMove = 3000;
		accuTimeTinyGenAnimation += elapsed_ms;
		if (accuTimeTinyGenAnimation < totalTimeToMove) {
			registry.foregroundMotions.get(tinyGen).position = vec2({ window_width_px + 50, window_height_px - 50 }) * (1 - (accuTimeTinyGenAnimation / totalTimeToMove)) + TINY_GEN_ENDING_POS * (accuTimeTinyGenAnimation / totalTimeToMove);
		}
		else {
			animationSevenFinished = true;
			animationSevenStarted = false;
		}
	}
}