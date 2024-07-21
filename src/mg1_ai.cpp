// internal
#include "mg1_ai.hpp"
#include <queue>
#include <unordered_set>
#include <SDL.h>
#include <array>
#include <iostream>

// Forces the AI to recompute path once it has finished its current path
bool FORCE_COMPUTE = true;
bool AI_STALLED = false;

// How "far" ahead the AI looks. Accounts for up, down, left, right
int STEP_SIZE = 100;

// The error tolerance for what the algo considers "found". Accounts for the fact the sprites aren't just one point
int TOLERANCE = 80;

// DISTANCE_LEFT_TO_TRAVEL keeps the AI moving in 100 step intervals so it's more on rails.
float DISTANCE_LEFT_TO_TRAVEL = 0;
int DIRECTION_INDEX = 0;
int AI_SPEED = 4;

void MiniGame1AI::step(float elapsed_ms)
{
 	float step_seconds = elapsed_ms / 1000.f;

	if (game_state != (unsigned int)GAME_STATES::MINIGAME_1 || pause_game_state == true) return;
	auto& motion_deadly = registry.deadlys;
	if (motion_deadly.entities.size() <= 0) return;
	// Recalculate path every COMPUTE_CYCLE tick or if FORCE_COMPUTE is true
	if (TICK % COMPUTE_CYCLE == 0 || FORCE_COMPUTE) {
 		if (DISTANCE_LEFT_TO_TRAVEL != 0) {
			// If there's still distance until the AI has moved 100 steps, don't recalculate yet. But indicate it will right after reaching 100 steps.
			AI_STALLED = true;
			FORCE_COMPUTE = false;
		}
		else {
			STEP_SIZE = 100;
			TOLERANCE = 80;
			Entity& player = registry.players.entities[0];
			int xTarget = registry.foregroundMotions.get(player).position.x;
			int yTarget = registry.foregroundMotions.get(player).position.y;

			// Currently hardcoded for only 1 deadly entity
			foregroundMotion& rbc_motion = registry.foregroundMotions.get(motion_deadly.entities[0]);
			int xStart = rbc_motion.position.x;
			int yStart = rbc_motion.position.y;
			motion_deadly.components[0].chase = generatePath(xStart, yStart, xTarget, yTarget).dir;
			if (motion_deadly.components[0].chase.size() == 0) {
				STEP_SIZE = 50;
				TOLERANCE = 45;
				motion_deadly.components[0].chase = generatePath(xStart, yStart, xTarget, yTarget).dir;
			}
			DISTANCE_LEFT_TO_TRAVEL = (float)STEP_SIZE;
			DIRECTION_INDEX = 0;
			FORCE_COMPUTE = false;
			AI_STALLED = false;
		}
	}
	else {
		// follow step plan
		foregroundMotion& rbc_motion = registry.foregroundMotions.get(motion_deadly.entities[0]);
		if (motion_deadly.components[0].chase.size() > DIRECTION_INDEX) {
			Direction dir = motion_deadly.components[0].chase[DIRECTION_INDEX];
			float scaleX = abs(rbc_motion.scale.x) / 2.0f;
			float scaleY = abs(rbc_motion.scale.y) / 2.0f;

			float distanceTraveled = AI_SPEED * step_seconds * 50.f;
			bool done_after_this_step = DISTANCE_LEFT_TO_TRAVEL - distanceTraveled <= 0;

			// bound the amount you increment to keep you on on the center.
			float bounded_distance = done_after_this_step ? DISTANCE_LEFT_TO_TRAVEL : distanceTraveled;

			switch (dir) {
				case UP:
					rbc_motion.position.y -= bounded_distance;
					DISTANCE_LEFT_TO_TRAVEL = max(0.f, DISTANCE_LEFT_TO_TRAVEL - distanceTraveled);
					if (done_after_this_step) {
						DIRECTION_INDEX++;
						if (AI_STALLED) {
							FORCE_COMPUTE = true;
							AI_STALLED = false;
						}
						else {
							DISTANCE_LEFT_TO_TRAVEL = (float)STEP_SIZE;
						}
					}
					break;
				case DOWN:
					rbc_motion.position.y += bounded_distance;
					DISTANCE_LEFT_TO_TRAVEL = max(0.f, DISTANCE_LEFT_TO_TRAVEL - distanceTraveled);
					if (done_after_this_step) {
						DIRECTION_INDEX++;
						if (AI_STALLED) {
							FORCE_COMPUTE = true;
							AI_STALLED = false;
						}
						else {
							DISTANCE_LEFT_TO_TRAVEL = (float)STEP_SIZE;
						}
					}
					break;
				case RIGHT:
					rbc_motion.position.x += bounded_distance;
					DISTANCE_LEFT_TO_TRAVEL = max(0.f, DISTANCE_LEFT_TO_TRAVEL - distanceTraveled);
					if (done_after_this_step) {
						DIRECTION_INDEX++;
						if (AI_STALLED) {
							FORCE_COMPUTE = true;
							AI_STALLED = false;
						}
						else {
							DISTANCE_LEFT_TO_TRAVEL = (float)STEP_SIZE;
						}
					}
					break;
				case LEFT:
					rbc_motion.position.x -= bounded_distance;
					DISTANCE_LEFT_TO_TRAVEL = max(0.f, DISTANCE_LEFT_TO_TRAVEL - distanceTraveled);
					if (done_after_this_step) {
						DIRECTION_INDEX++;
						if (AI_STALLED) {
							FORCE_COMPUTE = true;
							AI_STALLED = false;
						}
						else {
							DISTANCE_LEFT_TO_TRAVEL = (float)STEP_SIZE;
						}
					}
					break;
			}
			changeRotationAndDirection(rbc_motion, dir);
			// Bound the enemy ai to the game window
			rbc_motion.position.x = max(50.f, min((float)window_width_px - 50, rbc_motion.position.x));
			rbc_motion.position.y = max(50.f, min((float)window_height_px - 50, rbc_motion.position.y));
		}
		else {
			FORCE_COMPUTE = true;
			DISTANCE_LEFT_TO_TRAVEL = 0;
		}
	}
	TICK++;
}

// Runs A* pathfinding using the "Manhattan Distance" between start and target as heuristic
Search MiniGame1AI::generatePath(int xStart, int yStart, int xTarget, int yTarget) {
	// Initialize a minimum priority queue based on f-values (current path costs + calculated heuristic). Uses struct "Search".
	std::priority_queue<Search> pq;
	// Initialize visited nodes list
	std::unordered_set<SearchPosition, searchPositionHash> visited;
	// Initialize initial cost of 0 and corresponding Search Struct
	Search initial = Search(generateHeuristic(xStart, yStart, xTarget, yTarget), 0, SearchPosition(xStart, yStart));
	pq.push(initial);

	while (!pq.empty()) {
		// Dequeues the best f-value path from the pq
		Search const& path = pq.top();
		std::vector<Direction> currentDirection = path.dir;
		SearchPosition currentNode = path.lastNode;
		int pathCost = path.cost;
		int currX = path.lastNode.x;
		int currY = path.lastNode.y;


		// Checks if you're near the target
		if (abs(currX - xTarget) < TOLERANCE && abs(currY - yTarget) < TOLERANCE) {
			return path;
		}

		pq.pop();

		// If you visited already, ignore
		if (visited.count(currentNode)) continue;
		visited.insert(currentNode);


		// Below generates a new Search Struct for left, right, up, down and adds it to the PQ.
		// left
		if (currX >= STEP_SIZE && !collidesWithWallX({ currX - STEP_SIZE, currY }, 0)) {
			SearchPosition newNode = SearchPosition(currX - STEP_SIZE, currY);
			Search left = Search(generateHeuristic(currX - STEP_SIZE, currY, xTarget, yTarget) + pathCost, pathCost + STEP_SIZE, newNode);
			left.dir = currentDirection;
			left.dir.emplace_back(LEFT);
			pq.push(left);
		}

		// right
		if (currX + STEP_SIZE <= window_width_px && !collidesWithWallX({ currX + STEP_SIZE, currY }, 0)) {
			SearchPosition newNode = SearchPosition(currX + STEP_SIZE, currY);
			Search right = Search(generateHeuristic(currX + STEP_SIZE, currY, xTarget, yTarget) + pathCost, pathCost + STEP_SIZE, newNode);
			right.dir = currentDirection;
			right.dir.emplace_back(RIGHT);
			pq.push(right);
		}

		// up
		if (currY >= STEP_SIZE && !collidesWithWallY({ currX, currY - STEP_SIZE }, 0)) {
			SearchPosition newNode = SearchPosition(currX, currY - STEP_SIZE);
			Search up = Search(generateHeuristic(currX, currY - STEP_SIZE, xTarget, yTarget) + pathCost, pathCost + STEP_SIZE, newNode);
			up.dir = currentDirection;
			up.dir.emplace_back(UP);
			pq.push(up);
		}
		// down
		if (currY + STEP_SIZE <= window_height_px && !collidesWithWallY({ currX, currY + STEP_SIZE }, 0)) {
			SearchPosition newNode = SearchPosition(currX, currY + STEP_SIZE);
			Search down = Search(generateHeuristic(currX, currY + STEP_SIZE, xTarget, yTarget) + pathCost, pathCost + STEP_SIZE, newNode);
			down.dir = currentDirection;
			down.dir.emplace_back(DOWN);
			pq.push(down);
		}
	}
	// If you get here, there's no path found which theoretically couldn't happen.
	return Search(-1, -1, SearchPosition(0, 0));
}

int MiniGame1AI::generateHeuristic(int xStart, int yStart, int xTarget, int yTarget) {
	// Assuming only up and down for now. Uses Manhattan distance.
	return abs(xTarget - xStart) + abs(yTarget - yStart);
}

void MiniGame1AI::changeRotationAndDirection(foregroundMotion& motion, Direction& direction)
{
	float& angle = motion.angle;
	float& x_scale = motion.scale.x;

	switch (direction)
	{
	case UP:
		angle = M_PI / 2;
		break;
	case LEFT:
		if (x_scale > 0)
			x_scale *= -1;
		angle = 0;
		break;
	case RIGHT:
		if (x_scale < 0)
			x_scale *= -1;
		angle = 0;
		break;
	case DOWN:
		angle = -M_PI / 2;
		break;
	}
}