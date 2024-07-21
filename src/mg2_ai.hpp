#pragma once

#include "common_ai.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"

class MiniGame2AI : public CommonAI {
public:
	void step(float elapsed_ms);
	MiniGame2AI() {}

};

void initializeMG2Tree(RenderSystem* renderer);