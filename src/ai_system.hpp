#pragma once

#include "common_ai.hpp"
#include "mg1_ai.hpp"
#include "mg2_ai.hpp"

class AISystem {

public:

	void step(float elapsed_ms);
	AISystem() {
		dummyAI = new DummyAI();
		mg1AI = new MiniGame1AI();
		mg2AI = new MiniGame2AI();
	}

	~AISystem() {
		delete dummyAI;
		delete mg1AI;
		delete mg2AI;
	}

private:

	CommonAI* getAI();
	DummyAI* dummyAI;
	MiniGame1AI* mg1AI;
	MiniGame2AI* mg2AI;

};
