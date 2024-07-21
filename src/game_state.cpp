#include "game_state.hpp"
#include "json.hpp"
#include "common.hpp"

using json = nlohmann::json;

unsigned int game_state;
bool pause_game_state;

// Needs to be called everytime moving to a new game state
void GameState::newGameStateContainers() {
	ComponentContainer<foregroundMotion> motions;
	registry.foregroundMotions = motions;

	ComponentContainer<backgroundMotions> staticObjects;
	registry.backgroundMotions = staticObjects;

	ComponentContainer<overlayMotions> overlayMotions;
	registry.overlayMotions = overlayMotions;

	ComponentContainer<Collision> collisions;
	registry.collisions = collisions;

	ComponentContainer<Player> players;
	registry.players = players;

	ComponentContainer<Mesh*> meshPtrs;
	registry.meshPtrs = meshPtrs;

	ComponentContainer<RenderRequest> backgroundRenderRequests;
	registry.backgroundRenderRequests = backgroundRenderRequests;

	ComponentContainer<RenderRequest> foregroundRenderRequests;
	registry.foregroundRenderRequests = foregroundRenderRequests;

	ComponentContainer<RenderRequest> overlayRenderRequests;
	registry.overlayRenderRequests = overlayRenderRequests;

	ComponentContainer<TextRenderRequest> textRenderRequests;
	registry.textRenderRequests = textRenderRequests;

	//overworldRegistry.screenStates = registry.screenStates;
	//ComponentContainer<ScreenState> screenStates;
	//registry.screenStates = screenStates;

	ComponentContainer<Consumable> consumables;
	registry.consumables = consumables;

	ComponentContainer<Deadly> deadlys;
	registry.deadlys = deadlys;

	ComponentContainer<DebugComponent> debugComponents;
	registry.debugComponents = debugComponents;

	ComponentContainer<vec3> colors;
	registry.colors = colors;

	ComponentContainer<GameNode> nodes;
	registry.gameNodes = nodes;

	ComponentContainer<Collidable> collidables;
	registry.collidables = collidables;

	ComponentContainer<Arrow> arrows;
	registry.arrows = arrows;

	ComponentContainer<Background> background;
	registry.background = background;

	ComponentContainer<Wall> walls;
	registry.walls = walls;

	ComponentContainer<Animation> animation;
	registry.animation = animation;

	//ComponentContainer<Text> texts;
	//registry.texts = texts;

	ComponentContainer<WhackAMole> whackAMole;
	registry.whackAMole = whackAMole;

	ComponentContainer<Title> titleChoice;
	registry.title = titleChoice;

	ComponentContainer<InstanceRenderRequest> instanceRenderRequest;
	registry.instanceRenderRequests = instanceRenderRequest;

	ComponentContainer<SavedGameTimer> savedGameTimer;
	registry.savedGameTimer = savedGameTimer;

	ComponentContainer<Platform> platform;
	registry.platform = platform;
	
	ComponentContainer<Jump> jump;
	registry.jump = jump;

	ComponentContainer<Ball> balls;
	registry.balls = balls;

	ComponentContainer<Brick> bricks;
	registry.bricks = bricks;

	ComponentContainer<BezierCurve> beziers;
	registry.beziers = beziers;

	ComponentContainer<Particle> particles;
	registry.particles = particles;

	ComponentContainer<FinishLine> finishLine;
	registry.finishLine = finishLine;
	ComponentContainer<Paddle> paddles;
	registry.paddles = paddles;

	ComponentContainer<PowerUp> powerUps;
	registry.powerUps = powerUps;

}

void GameState::read_save_state() {
	std::string filePath = data_path() + "/saveFile.json";
	std::ifstream saveFile(filePath);

	// Success
	if (saveFile) {
		saveStatePersistence = json::parse(saveFile);
		saveFile.close();
	}
	else {
		printf("ERROR: COULD NOT LOAD SAVE FILE.  CREATING NOW.\n");
		saveFile.close();
		initStatePersistence();
	}
}

void GameState::write_save_state() {
	std::string filePath = data_path() + "/saveFile.json";
	std::ofstream saveWriteFile(filePath);
	saveWriteFile << saveStatePersistence;
	saveWriteFile.close();
}


void GameState::initStatePersistence() {
	json ex3 = {
		{"currentPositionX", 0.00},
		{"currentPositionY", 0.00},
		{"currentOrgan", 0},
		{"unlockedOrgan", 0},
		{"seenTutorials", {
			{GAME_STATES::TITLE, false},
			{GAME_STATES::MINIGAME_1, false},
			{GAME_STATES::MINIGAME_2, false},
			{GAME_STATES::MINIGAME_3, false},
			{GAME_STATES::MINIGAME_4, false},
			{GAME_STATES::MINIGAME_5, false}
			}
		},
		{"items", {
			{"atp", 0},
			{"lipids", 0},
			{"glucose", 0},
			{"iron", 0},
			{"oxygen", 0}
			}
		}
	};
	saveStatePersistence = ex3;

}