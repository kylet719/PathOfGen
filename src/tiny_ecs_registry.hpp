#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<foregroundMotion> foregroundMotions;
	ComponentContainer<backgroundMotions> backgroundMotions;
	ComponentContainer<overlayMotions> overlayMotions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> backgroundRenderRequests; // For backgrounds and static objects
	ComponentContainer<RenderRequest> foregroundRenderRequests; // For moving objects that tend to be in the foreground
	ComponentContainer<RenderRequest> overlayRenderRequests;	// For objects that need to stay on the front of the screen
	ComponentContainer<TextRenderRequest> textRenderRequests;   // For text to be rendered in front of overlay layer
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Consumable> consumables;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<GameNode> gameNodes;
	ComponentContainer<TransportNode> transportNodes;
	ComponentContainer<Collidable> collidables;
	ComponentContainer<Arrow> arrows;
	ComponentContainer<Background> background;
	ComponentContainer<Wall> walls;
	ComponentContainer<Animation> animation;
	ComponentContainer<Text> texts;
	ComponentContainer<WhackAMole> whackAMole;
	ComponentContainer<Title> title;
	ComponentContainer<Credits> credits;
	ComponentContainer<SavedGameTimer> savedGameTimer;
	ComponentContainer<Bar> bar;
	ComponentContainer<Random> random;
	ComponentContainer<InstanceRenderRequest> instanceRenderRequests; // For performing instance rendering on a single entity
	ComponentContainer<Platform> platform;
	ComponentContainer<Jump> jump;
	ComponentContainer<Ball> balls;
	ComponentContainer<Brick> bricks;
	ComponentContainer<BezierCurve> beziers;
	ComponentContainer<Particle> particles;
	ComponentContainer<BrainItemCheckNode> brainItemCheckNode;
	ComponentContainer<BrainEndingChoiceNode> brainEndingChoiceNode;
	ComponentContainer<PowerUp> powerUps;
	ComponentContainer<Paddle> paddles;

	ComponentContainer<FinishLine> finishLine;
	// IMPORTANT:  When adding new components to the registry, be sure to also change GameState::save_overworld_state
	

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&foregroundMotions);
		registry_list.push_back(&backgroundMotions);
		registry_list.push_back(&overlayMotions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&backgroundRenderRequests);
		registry_list.push_back(&foregroundRenderRequests);
		registry_list.push_back(&overlayRenderRequests);
		registry_list.push_back(&textRenderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&consumables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&gameNodes);
		registry_list.push_back(&transportNodes);
		registry_list.push_back(&collidables);
		registry_list.push_back(&arrows);
		registry_list.push_back(&background);
		registry_list.push_back(&walls);
		registry_list.push_back(&animation);
		registry_list.push_back(&texts);
		registry_list.push_back(&whackAMole);
		registry_list.push_back(&title);
		registry_list.push_back(&credits);
		registry_list.push_back(&savedGameTimer);
		registry_list.push_back(&bar);
		registry_list.push_back(&random);
		registry_list.push_back(&instanceRenderRequests);
		registry_list.push_back(&platform);
		registry_list.push_back(&jump);
		registry_list.push_back(&balls);
		registry_list.push_back(&bricks);
		registry_list.push_back(&beziers);
		registry_list.push_back(&brainItemCheckNode);
		registry_list.push_back(&brainEndingChoiceNode);
		registry_list.push_back(&particles);
		registry_list.push_back(&finishLine);
		registry_list.push_back(&powerUps);
		registry_list.push_back(&paddles);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}

};

extern ECSRegistry registry;
