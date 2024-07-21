#pragma once

#include "common_ai.hpp"
#include "tiny_ecs_registry.hpp"

#include <vector>

struct SearchPosition
{
	int x, y;
	SearchPosition(int initX, int initY) : x(initX), y(initY) {}

	bool operator==(const SearchPosition& other) const {
		return x == other.x && y == other.y;
	}
};

struct searchPositionHash {
public:
	size_t operator()(const SearchPosition p) const
	{
		return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
	}
};

struct Search {
	int fvalue;
	int cost;
	SearchPosition lastNode;
	std::vector<Direction> dir;
	Search(int val, int costs, SearchPosition newNode) : fvalue(val), cost(costs), lastNode(newNode) {}

	bool operator<(const Search& other) const {
		return fvalue > other.fvalue;
	}
};



class MiniGame1AI : public CommonAI {
public:
	void step(float elapsed_ms);
	MiniGame1AI() {}

private:
	Search generatePath(int xStart, int yStart, int xTarget, int yTarget);
	int generateHeuristic(int xStart, int yStart, int xTarget, int yTarget);
	void changeRotationAndDirection(foregroundMotion& motion, Direction& direction);
};