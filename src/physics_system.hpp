#pragma once

#include "common_physics.hpp"
#include "title_physics.hpp"
#include "mg1_physics.hpp"
#include "mg2_physics.hpp"
#include "mg3_physics.hpp"
#include "organ_physics.hpp"
#include "mg4_physics.hpp"
#include "mg5_physics.hpp"
#include "credits_physics.hpp"

class PhysicsSystem {

public:

	void step(float elapsed_ms);
	PhysicsSystem() {
		titlePhysics = new TitlePhysics();
		organPhysics = new OrganPhysics();
		mg1Physics = new MiniGame1Physics();
		mg2Physics = new MiniGame2Physics();
		mg3Physics = new MiniGame3Physics();
		mg4Physics = new MiniGame4Physics();
		mg5Physics = new MiniGame5Physics();
		creditsPhysics = new CreditsPhysics();
	}

	~PhysicsSystem() {
		delete titlePhysics;
		delete organPhysics;
		delete mg1Physics;
		delete mg2Physics;
		delete mg3Physics;
		delete mg4Physics;
		delete mg5Physics;
		delete creditsPhysics;
	}

private:

	CommonPhysics* getPhysics();
	TitlePhysics* titlePhysics;
	OrganPhysics* organPhysics;
	MiniGame1Physics* mg1Physics;
	MiniGame2Physics* mg2Physics;
	MiniGame3Physics* mg3Physics;
	MiniGame4Physics* mg4Physics;
	MiniGame5Physics* mg5Physics;
	CreditsPhysics* creditsPhysics;

};
