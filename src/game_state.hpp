#pragma once

#include "tiny_ecs_registry.hpp"
#include "json.hpp"
using json = nlohmann::json;

class GameState
{
	
public:
	
	GameState(){
	}

	void newGameStateContainers();
	json saveStatePersistence;
	void read_save_state();
	void write_save_state();
	void initStatePersistence();

private:

	
};
