#include "mg5_physics.hpp"
#include "particle_system.hpp"

void MiniGame5Physics::step(float elapsed_ms) {

	float step_seconds = elapsed_ms / 1000.f;
	
	Entity& paddle = registry.paddles.entities[0];
	foregroundMotion& paddleMotion = registry.foregroundMotions.get(paddle);
	paddleMovementHandler(paddleMotion, step_seconds);
	
	// consumable movement
	for (Entity& consumableEntity: registry.consumables.entities) {

		if (registry.powerUps.has(consumableEntity)) {
			
			foregroundMotion& motion = registry.foregroundMotions.get(consumableEntity);
			motion.position += motion.velocity * step_seconds;
			if (checkBoxCollision(consumableEntity, paddle)) registry.collisions.emplace(consumableEntity, paddle);

		} else {

			handleOxygenMotion(consumableEntity, step_seconds);
			if (checkBoxCollision(consumableEntity, paddle)) registry.collisions.emplace(consumableEntity, paddle);
		}
	}

	// move the ball
	for (Entity& ballEntity : registry.balls.entities) {
		foregroundMotion& motion = registry.foregroundMotions.get(ballEntity);
		motion.position += motion.velocity * step_seconds;
		checkForBounce(ballEntity);
	}

	stepParticles(elapsed_ms);
}

void MiniGame5Physics::checkForBounce(Entity& ballEntity) {
	foregroundMotion& ballMotion = registry.foregroundMotions.get(ballEntity);
	BBox ballBBox = getBBoxBounds(ballEntity);

	checkForCollisions(ballEntity);

	// check window walls 
	if (ballBBox.right > window_width_px) { // right border
		ballMotion.position.x = window_width_px - ballMotion.scale.x / 2;
		ballMotion.velocity.x *= -1;
	} else if (ballBBox.left < 0.0f) { // left border
		ballMotion.position.x = ballMotion.scale.x / 2;
		ballMotion.velocity.x *= -1;
	} else if (ballBBox.top < 0.0f) { // top border
		ballMotion.position.y = (-ballMotion.scale.y) / 2;
		ballMotion.velocity.y *= -1;
	} else if (ballBBox.top > window_height_px) { // if ball passed the bottom of the screen
		registry.remove_all_components_of(ballEntity);
	}

}

void MiniGame5Physics::checkForCollisions(Entity& ballEntity) {
	Entity& paddle = registry.paddles.entities[0];

	// check for paddle collisions
	if (checkBoxCollision(paddle, ballEntity)) {
		handlePaddleCollision(paddle, ballEntity);
	}

	for (Entity& brickEntity : registry.bricks.entities) {
		if (checkBoxCollision(brickEntity, ballEntity)) {
			handleBrickCollision(brickEntity, ballEntity);
			registry.collisions.emplace_with_duplicates(brickEntity, ballEntity);
		}
	}

}

void handlePaddleTopReflection(foregroundMotion& ball, BBox& paddleBBox) {
	float paddleLength = paddleBBox.right - paddleBBox.left;
	float paddleSection = paddleLength / 5;
	float leftDivider = paddleBBox.left + paddleSection;
	float leftCenterDivider = paddleBBox.left + 2 * paddleSection;
	float rightCenterDivider = paddleBBox.right - 2 * paddleSection;
	float rightDivider = paddleBBox.right - paddleSection;

	if (ball.position.x < leftDivider) {
		ball.velocity = vec2(-350.f, -250.f);
	}
	if (ball.position.x > leftDivider && ball.position.x < leftCenterDivider) {
		ball.velocity = vec2(-350.f, -350.f);
	}
	if (ball.position.x > leftCenterDivider && ball.position.x < rightCenterDivider) {
		ball.velocity = vec2(ball.velocity.x / 2.f, -350.f);
	}
	if (ball.position.x > rightCenterDivider && ball.position.x < rightDivider) {
		ball.velocity = vec2(350.f, -350.f);
	}
	if (ball.position.x > rightDivider) {
		ball.velocity = vec2(350.f, -250.f);
	}

}

void MiniGame5Physics::handlePaddleCollision(Entity& paddleEntity, Entity& ballEntity) {
	foregroundMotion& ball = registry.foregroundMotions.get(ballEntity);
	BBox paddleBBox = CommonPhysics::getBBoxBounds(paddleEntity);

	if (ball.position.y < paddleBBox.top) {
		handlePaddleTopReflection(ball, paddleBBox);
	}
	if (ball.position.x < paddleBBox.left) {
		ball.velocity.x = abs(ball.velocity.x) * (-1);
	}
	if (ball.position.x > paddleBBox.right) {
		ball.velocity.x = abs(ball.velocity.x);
	}

}

void MiniGame5Physics::handleBrickCollision(Entity& brickEntity, Entity& ballEntity) {
	foregroundMotion& ball = registry.foregroundMotions.get(ballEntity);
	BBox brickBBox = CommonPhysics::getBBoxBounds(brickEntity);

	if (ball.position.y < brickBBox.top) {
		ball.velocity.y = abs(ball.velocity.y) * (-1);
	}
	if (ball.position.y > brickBBox.bottom) {
		ball.velocity.y = abs(ball.velocity.y);
	} 
	if (ball.position.x < brickBBox.left) {
		ball.velocity.x = abs(ball.velocity.x) * (-1);
	}
	if (ball.position.x > brickBBox.right) {
		ball.velocity.x = abs(ball.velocity.x);
	}

}

void MiniGame5Physics::handleOxygenMotion(Entity& oxygenEntity, float step_seconds) {
	foregroundMotion& motion = registry.foregroundMotions.get(oxygenEntity);
	BezierCurve& bezier = registry.beziers.get(oxygenEntity);
	bezier.t = fmin(bezier.t + step_seconds / 2.f, 1.0f);

	motion.position = getBezierPosition(bezier);

	removeOffScreen(oxygenEntity);
}

void MiniGame5Physics::removeOffScreen(Entity& entity) {
	foregroundMotion& motion = registry.foregroundMotions.get(entity);
	if (motion.position.y - abs(motion.scale.y) > window_height_px) {
		registry.remove_all_components_of(entity);
	}
}

void MiniGame5Physics::paddleMovementHandler(foregroundMotion& paddleMotion, float step_seconds) {
	for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
		if (keymap[i]) {
			switch (i) {
			case GLFW_KEY_A:
				paddleMotion.position.x += -PADDLE_STEP * step_seconds * 40.f;
				break;
			case GLFW_KEY_D:
				paddleMotion.position.x += PADDLE_STEP * step_seconds * 40.f;
				break;
			}
		}
		// Bounds the player in the game screen
		paddleMotion.position.x = max(paddleMotion.scale.x / 2, min((float)window_width_px - paddleMotion.scale.x / 2, paddleMotion.position.x));
	}
}
