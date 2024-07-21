#pragma once

#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

struct AITreeNode {
	// if isEnd is true, it's a leaf node, do Action, else check Decision function to go left or right
	bool isEnd;
	std::function<bool()> Decision;
	AITreeNode* left;
	AITreeNode* right;
	std::function<void()> Action;
};

class CommonAI {
public:
	virtual void step(float elapsed_ms) = 0;
protected:
	void processNode(AITreeNode* node);
	uint TICK = 0;
	int COMPUTE_CYCLE = 250;
};

class DummyAI : public CommonAI {
public:
	void step(float elapsed_ms) { return; }
	DummyAI() {}
};
