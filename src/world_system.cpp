// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "game_state.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include "mg2_ai.hpp"
#include "particle_system.hpp"


// Transport node positions
// NODE_POS_1_TO_2 indicates player is in organ 1, and this node takes them to organ 2
// This also means that when going from organ 2 to 1, the player will end up at this position.
const vec2 NODE_POS_1_TO_2 = vec2(window_width_px - 400, 100);
const vec2 NODE_POS_2_TO_1 = vec2(950, window_height_px - 200);
const vec2 NODE_POS_2_TO_3 = vec2(850, 100);
const vec2 NODE_POS_3_TO_2 = vec2(400, window_height_px - 200);
const vec2 NODE_POS_3_TO_4 = vec2(1000, 300);
const vec2 NODE_POS_4_TO_3 = vec2(400, window_height_px - 400);
const vec2 NODE_POS_4_TO_5 = vec2(window_width_px - 300,  150);
const vec2 NODE_POS_5_TO_4 = vec2(500, window_height_px - 150);
const vec2 NODE_POS_5_TO_BRAIN = vec2(window_width_px / 2 + 200, window_height_px -200);
const vec2 NODE_POS_BRAIN_TO_5 = vec2(window_width_px / 2 - 400, window_height_px -200);

RollingFps fpsWindow(60);

// Create the world
WorldSystem::WorldSystem(): points(0){
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (title_background_music != nullptr)
		Mix_FreeMusic(title_background_music);
	if (mg1_die_sound != nullptr)
		Mix_FreeChunk(mg1_die_sound);
	if (mg1_pickup_atp_sound != nullptr)
		Mix_FreeChunk(mg1_pickup_atp_sound);
	if (overworld_on_node_sound != nullptr)
		Mix_FreeChunk(overworld_on_node_sound);
	if (mg1_background_music != nullptr)
		Mix_FreeMusic(mg1_background_music);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Path of Gen", glfwGetPrimaryMonitor(), nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto cursor_click_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, cursor_click_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	title_background_music = Mix_LoadMUS(audio_path("title_background_music.wav").c_str());
	title_select_sound = Mix_LoadWAV(audio_path("title_select_sound.wav").c_str());
	title_enter_sound = Mix_LoadWAV(audio_path("title_enter_sound.wav").c_str());
	organ1_background_music = Mix_LoadMUS(audio_path("organ1_background_music.wav").c_str());
	mg1_die_sound = Mix_LoadWAV(audio_path("mg1_die.wav").c_str());
	mg1_pickup_atp_sound = Mix_LoadWAV(audio_path("mg1_pickup_atp.wav").c_str());
	overworld_on_node_sound = Mix_LoadWAV(audio_path("overworld_on_node.wav").c_str());
	mg1_background_music = Mix_LoadMUS(audio_path("mg1_background_music.wav").c_str());
	organ2_background_music = Mix_LoadMUS(audio_path("organ2_background_music.wav").c_str());
	mg2_background_music = Mix_LoadMUS(audio_path("mg2_background_music.wav").c_str());
	organ3_background_music = Mix_LoadMUS(audio_path("organ3_background_music.wav").c_str());
	mg3_background_music = Mix_LoadMUS(audio_path("mg3_background_music.wav").c_str());
	mg3_whack_sound = Mix_LoadWAV(audio_path("mg3_whack_sound.wav").c_str());
	mg3_explosion_sound = Mix_LoadWAV(audio_path("mg3_explosion_sound.wav").c_str());
	organ4_background_music = Mix_LoadMUS(audio_path("organ4_background_music.wav").c_str());
	mg4_background_music = Mix_LoadMUS(audio_path("mg4_background_music.wav").c_str());
	save_game_sound = Mix_LoadWAV(audio_path("save_game_sound.wav").c_str());
	organ5_background_music = Mix_LoadMUS(audio_path("organ5_background_music.wav").c_str());
	mg5_background_music = Mix_LoadMUS(audio_path("mg5_background_music.wav").c_str());
	mg5_paddle_increase_sound = Mix_LoadWAV(audio_path("mg5_paddle_increase.wav").c_str());
	mg5_x3_multiplier_sound = Mix_LoadWAV(audio_path("mg5_x3_multiplier.wav").c_str());
	credits_background_music = Mix_LoadMUS(audio_path("credits_background_music.wav").c_str());
	brain_background_music = Mix_LoadMUS(audio_path("brain_background_music.wav").c_str());
	brain_kill_background_music = Mix_LoadMUS(audio_path("brain_kill_background_music.wav").c_str());
	brain_help_background_music = Mix_LoadMUS(audio_path("brain_help_background_music.wav").c_str());


	if (title_background_music == nullptr ||
		title_select_sound == nullptr ||
		title_enter_sound == nullptr ||
		organ1_background_music == nullptr ||
		mg1_die_sound == nullptr ||
		mg1_pickup_atp_sound == nullptr ||
		overworld_on_node_sound == nullptr ||
		mg1_background_music == nullptr ||
		organ2_background_music == nullptr ||
		mg2_background_music == nullptr ||
		organ3_background_music == nullptr ||
		mg3_background_music == nullptr ||
		mg3_explosion_sound == nullptr ||
		organ4_background_music == nullptr ||
		mg4_background_music == nullptr ||
		save_game_sound == nullptr ||
		organ5_background_music == nullptr ||
		mg5_background_music == nullptr ||
		mg5_paddle_increase_sound == nullptr ||
		mg5_x3_multiplier_sound == nullptr ||
		credits_background_music == nullptr ||
		brain_kill_background_music == nullptr ||
		brain_help_background_music == nullptr ||
		brain_background_music == nullptr 
		) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("title_background_music.wav").c_str(),
			audio_path("title_select_sound.wav").c_str(),
			audio_path("organ1_background_music.wav").c_str(),
			audio_path("mg1_background_music.wav").c_str(),
			audio_path("mg1_die.wav").c_str(),
			audio_path("mg1_pickup_atp.wav").c_str(),
			audio_path("overworld_on_node.wav").c_str(),
			audio_path("organ2_background_music.wav").c_str(),
			audio_path("mg2_background_music.wav").c_str(),
			audio_path("organ3_background_music.wav").c_str(),
			audio_path("mg3_background_music.wav").c_str(),
			audio_path("mg3_whack_sound.wav").c_str(),
			audio_path("mg3_explosion_sound.wav").c_str(),
			audio_path("organ4_background_music.wav").c_str(),
			audio_path("mg4_background_music.wav").c_str(),
			audio_path("save_game_sound.wav").c_str(),
			audio_path("organ5_background_music.wav").c_str(),
			audio_path("mg5_background_music.wav").c_str(),
			audio_path("mg5_paddle_increase_sound.wav").c_str(),
			audio_path("mg5_x3_multiplier_sound.wav").c_str(),
			audio_path("credits_background_music.wav").c_str(),
			audio_path("brain_kill_background_music.wav").c_str(),
			audio_path("brain_help_background_music.wav").c_str(),
			audio_path("brain_background_music.wav").c_str()
		);
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Set all states to default
	restart_game();
}

// Update our game world - Focus on the environment and the game system like the window and screen
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Path of Gen";

	if (debugging.in_debug_mode) {
		fpsWindow.update(elapsed_ms_since_last_update);
		float fps = fpsWindow.calculate_fps();
		title_ss << "   fps: " << std::round(fps);

		if (counter >= 5) {
			std::string fpsString = std::to_string((int)fps);
			Text& text = registry.texts.get(fpsTextEntity);
			text.str = "FPS: " + fpsString;
			
			counter = 0;
		} else {
			counter++;
		}
	} else {
		Text& text = registry.texts.get(fpsTextEntity);
		text.str = "";
	}

	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0) {
		registry.remove_all_components_of(registry.debugComponents.entities.back());
	}

	// Processing the screen state
  	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	end_minigame_2(elapsed_ms_since_last_update);
	minigame3_step(elapsed_ms_since_last_update);
	minigame4_step(elapsed_ms_since_last_update);
	end_minigame_4();
	if (game_state == (unsigned int)GAME_STATES::MINIGAME_5 && pause_game_state == false) minigame5_step(elapsed_ms_since_last_update);

	for (Entity entity : registry.savedGameTimer.entities) {
		SavedGameTimer& counter = registry.savedGameTimer.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < 1) {
			registry.remove_all_components_of(entity);
		}
	}

	for (Entity animation_entity: registry.animation.entities)
	{
		if (registry.overlayRenderRequests.has(animation_entity))
		{
			Animation& animation = registry.animation.get(animation_entity);

			if (animation.current_frame < animation.total_frames - 1)
			{
				animation.elapsed_ms -= elapsed_ms_since_last_update;
				if (animation.elapsed_ms < 0 && animation.current_x_frame < animation.columns - 1)
				{
					animation.current_frame++;
					animation.current_x_frame++;
					animation.elapsed_ms = 100.f;
				}
				else if (animation.elapsed_ms < 0 && animation.current_x_frame == animation.columns - 1)
				{
					if (animation.current_y_frame < animation.rows)
					{
						animation.current_frame++;
						animation.current_x_frame = 0;
						animation.elapsed_ms = 100.f;
						animation.current_y_frame++;
					}
				}
			}
			else if(inEndingScene) {
				endingSceneFinished = true;
			}
		} 
		else if (registry.deadlys.has(animation_entity))
		{
			Animation& animation = registry.animation.get(animation_entity);

			animation.elapsed_ms -= elapsed_ms_since_last_update;
			if (animation.elapsed_ms < 0 && animation.current_x_frame < animation.columns - 1)
			{
				animation.current_x_frame++;
				animation.elapsed_ms = 75.f;
			}
			else if (animation.elapsed_ms < 0 && animation.current_x_frame == animation.columns - 1)
			{
				animation.current_x_frame = 0;
				animation.elapsed_ms = 75.f;
			}
		}
	}

	float &fadeOutTimer = registry.screenStates.get(renderer->screen_state_entity).fadeOutTimer;

	if (fadeOut) fadeOutTimer -= elapsed_ms_since_last_update;
	screen.darken_screen_factor = 1 - fadeOutTimer / 1500;

	if (fadeOut && game_state == (unsigned int)GAME_STATES::TITLE && registry.title.components[0].titleInPlace) {
		if (fadeOutTimer <= 0) {
			pause_game_state = false;
			if (registry.title.components[0].selectionOption == (unsigned int)TITLE_OPTIONS_SELECTION::NEWGAME) {
				// New game inits
				player_saved_pos = vec2(window_width_px - window_width_px / 3, window_height_px - 300);
				mg1_inv_points = mg2_inv_points = mg3_inv_points = mg4_inv_points = mg5_inv_points = 0;
				showGoal = false;
				brainGateUnlocked = false;
				enoughItems = false;
				completedOrgan = (unsigned int)GAME_STATES::TITLE;
				change_game_states(GAME_STATES::ORGAN_1);
			}
			else if (registry.title.components[0].selectionOption == (unsigned int)TITLE_OPTIONS_SELECTION::LOAD) {
				load_game_saveFile();
			}
			else {
				change_game_states(GAME_STATES::CREDITS);
			}
			fadeOut = false;
			fadeOutTimer = 1500;
		}
	}

	if (fadeOut && endingSceneFinished) {
		if (fadeOutTimer <= 0) {
			pause_game_state = false;
			change_game_states(GAME_STATES::CREDITS);
			inEndingScene = false;
			endingSceneFinished = false;
			fadeOut = false;
			fadeOutTimer = 1500;
		}
	}

	if (fadeOut && game_state == (unsigned int)GAME_STATES::CREDITS) {
		if (registry.credits.components[0].creditsFinished) {
			if (fadeOutTimer <= 0) {
				pause_game_state = false;
				change_game_states(GAME_STATES::TITLE);
				registry.credits.components[0].creditsFinished = false;
				registry.credits.components[0].creditsStarted = false;
				fadeOut = false;
				fadeOutTimer = 1500;
			}
		}
	}

	if (fadeOut &&
		game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		) {
		if (fadeOutTimer <= 0) {
			pause_game_state = false;
			auto& collisionsRegistry = registry.collisions;
			for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
				// The entity and its collider
				Entity entity = collisionsRegistry.entities[i];
				Entity entity_other = collisionsRegistry.components[i].other;

				if (registry.gameNodes.has(entity_other)) {
					if (registry.gameNodes.get(entity_other).minigame == (unsigned int)GAME_STATES::MINIGAME_1) {
						// Enters minigame 1
						player_saved_pos = registry.foregroundMotions.get(player).position;
						change_game_states(GAME_STATES::MINIGAME_1);
					}
					else if (registry.gameNodes.get(entity_other).minigame == (unsigned int)GAME_STATES::MINIGAME_2) {
						// Enters minigame 2
						player_saved_pos = registry.foregroundMotions.get(player).position;
						change_game_states(GAME_STATES::MINIGAME_2);
					}
					else if (registry.gameNodes.get(entity_other).minigame == (unsigned int)GAME_STATES::MINIGAME_3) {
						// Enters minigame 3
						player_saved_pos = registry.foregroundMotions.get(player).position;
						change_game_states(GAME_STATES::MINIGAME_3);
					}
					else if (registry.gameNodes.get(entity_other).minigame == (unsigned int)GAME_STATES::MINIGAME_4)
					{
						player_saved_pos = registry.foregroundMotions.get(player).position;
						change_game_states(GAME_STATES::MINIGAME_4);
					}
					else if (registry.gameNodes.get(entity_other).minigame == (unsigned int)GAME_STATES::MINIGAME_5) {
						// Enters minigame 5
						player_saved_pos = registry.foregroundMotions.get(player).position;
						change_game_states(GAME_STATES::MINIGAME_5);
					}
				} else if (registry.transportNodes.has(entity_other)) {
					if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::ORGAN_1) {
						if (currentOrgan == GAME_STATES::ORGAN_2) {
							player_saved_pos = NODE_POS_1_TO_2;
						}
						change_game_states(GAME_STATES::ORGAN_1);
					}
					else if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::ORGAN_2) {
						if (currentOrgan == GAME_STATES::ORGAN_1) {
							player_saved_pos = NODE_POS_2_TO_1;
						}
						else if (currentOrgan == GAME_STATES::ORGAN_3) {
							player_saved_pos = NODE_POS_2_TO_3;
						}
						change_game_states(GAME_STATES::ORGAN_2);
					}
					else if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::ORGAN_3) {
						if (currentOrgan == GAME_STATES::ORGAN_2) {
							player_saved_pos = NODE_POS_3_TO_2;
						}
						else if (currentOrgan == GAME_STATES::ORGAN_4) {
							player_saved_pos = NODE_POS_3_TO_4;
						}
						change_game_states(GAME_STATES::ORGAN_3);
					}
					else if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::ORGAN_4) {
						if (currentOrgan == GAME_STATES::ORGAN_3) {
							player_saved_pos = NODE_POS_4_TO_3;
						}
						else if (currentOrgan == GAME_STATES::ORGAN_5) {
							player_saved_pos = NODE_POS_4_TO_5;
						}
						change_game_states(GAME_STATES::ORGAN_4);
					}
					else if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::ORGAN_5) {
						if (currentOrgan == GAME_STATES::ORGAN_4) {
							player_saved_pos = NODE_POS_5_TO_4;
						}
						else if (currentOrgan == GAME_STATES::BRAIN_LOCKED || currentOrgan == GAME_STATES::BRAIN_UNLOCKED) {
							player_saved_pos = NODE_POS_5_TO_BRAIN;
						}
						change_game_states(GAME_STATES::ORGAN_5);
					}
					else if (registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
						registry.transportNodes.get(entity_other).nextOrgan == (unsigned int)GAME_STATES::BRAIN_UNLOCKED) {
						if (currentOrgan == GAME_STATES::ORGAN_5) {
							player_saved_pos = NODE_POS_BRAIN_TO_5;
						}
						change_game_states(GAME_STATES::BRAIN_LOCKED);
					}
				} else if (registry.brainEndingChoiceNode.has(entity_other)) {
					BrainEndingChoiceNode endingNode = registry.brainEndingChoiceNode.has(collisionsRegistry.entities[0]) ? registry.brainEndingChoiceNode.get(collisionsRegistry.entities[0]) : registry.brainEndingChoiceNode.get(collisionsRegistry.entities[1]);
					if (endingNode.killEnding) {
						change_game_states(GAME_STATES::BRAIN_KILL);
					}
					else {
						change_game_states(GAME_STATES::BRAIN_HELP);
					}
				}
			}
			fadeOut = false;
			fadeOutTimer = 1500;
		}
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the current game state
	game_state = (unsigned int)GAME_STATES::TITLE;
	
	// Remove all entities that we created
	// All that have a motion
	while (registry.foregroundMotions.entities.size() > 0) {
		registry.remove_all_components_of(registry.foregroundMotions.entities.back());
		
	}

	// Debugging for memory/component leaks
	registry.list_all_components();
	game_state_system.initStatePersistence();
	completedOrgan = (unsigned int) GAME_STATES::TITLE;
	create_title();
}

void WorldSystem::create_title() {
	Mix_PlayMusic(title_background_music, -1);
	fpsTextEntity = createFpsText();

	auto entity = Entity();
	registry.title.emplace(entity);

	game_state = (unsigned int)GAME_STATES::TITLE;

	createBackground(renderer, TEXTURE_ASSET_ID::TITLE_BACKGROUND, { window_width_px / 2,  -window_height_px}, { window_width_px/2, -window_height_px*2-300});
	createBackground(renderer, TEXTURE_ASSET_ID::TITLE_SPLASH, { window_width_px / 2,  -500 }, {300, -300 }); // Not technically a background but a good hack to make it work
	createBackground(renderer, TEXTURE_ASSET_ID::TITLE_ARROW, { window_width_px / 2 -325,  -100 }, { 50, 50 }); // Not technically a background but a good hack to make it work
	createBackground(renderer, TEXTURE_ASSET_ID::TITLE_OPTIONS, { window_width_px / 2 + 25,  -100 }, { 207, -94 }); // Not technically a background but a good hack to make it work
	createBackground(renderer, TEXTURE_ASSET_ID::TITLE_CONTROLS, { 110,  -100 }, { 100, -50 }); // Not technically a background but a good hack to make it work
}

void WorldSystem::create_credits() {
	fpsTextEntity = createFpsText();
	Mix_PlayMusic(credits_background_music, 0);
	auto entity = Entity();
	registry.credits.emplace(entity);
	game_state = (unsigned int)GAME_STATES::CREDITS;
	createBackground(renderer, TEXTURE_ASSET_ID::CREDITS_BG, { window_width_px / 2,  7500/2 }, { window_width_px/2, -7500/2});
}

// HOW TO ADD AN ORGAN
// - Add new GAME_STATE enum
// - Make a new "create_organ" method
// - Ensure you have transport nodes and their positions relative to each other
// - Adjust the SPACE key handling such that when on a transport node, it adjust the player position before changing the game state to the next organ
// - Add a new CASE to the physics_system.cpp and ai_system.cpp
// - Add the new GAME_STATE enum to the OrganPhysics::handleNodeCollisions(), and SPACE key handler if conditions

void WorldSystem::create_organ_1(vec2 startingPos) {
	Mix_PlayMusic(organ1_background_music, -1);
	fpsTextEntity = createFpsText();
	currentOrgan = GAME_STATES::ORGAN_1;
	game_state = (unsigned int)GAME_STATES::ORGAN_1;
	createBackground(renderer, TEXTURE_ASSET_ID::ORGAN_1_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px/2 });

	Entity node = createMinigameNode(renderer, vec2(window_width_px / 2, window_height_px / 2), GAME_STATES::MINIGAME_1);
	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });

	if ((unsigned int)completedOrgan >= (unsigned int)currentOrgan) {
		// Minigame has been beaten here
		createTransportNode(NODE_POS_1_TO_2, GAME_STATES::ORGAN_2);
		registry.colors.insert(node, vec3(0.1f, 1.0f, 0.8f));
	}
	if (!tutorialChecklist[GAME_STATES::TITLE]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::OVERALL_TUTORIAL);
		tutorialChecklist[GAME_STATES::TITLE] = true;
	}
	display_inventory();
}

void WorldSystem::create_organ_2(vec2 startingPos) {
	Mix_PlayMusic(organ2_background_music, -1);
	fpsTextEntity = createFpsText();
	currentOrgan = GAME_STATES::ORGAN_2;
	game_state = (unsigned int)GAME_STATES::ORGAN_2;
	createBackground(renderer, TEXTURE_ASSET_ID::ORGAN_2_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	Entity node = createMinigameNode(renderer, vec2(window_width_px / 2, window_height_px / 2), GAME_STATES::MINIGAME_2);
	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });
	createTransportNode(NODE_POS_2_TO_1, GAME_STATES::ORGAN_1);

	if ((unsigned int)completedOrgan >= (unsigned int)currentOrgan) {
		// Minigame has been beaten here
		createTransportNode(NODE_POS_2_TO_3, GAME_STATES::ORGAN_3);
		registry.colors.insert(node, vec3(0.1f, 1.0f, 0.8f));
	}
	display_inventory();
}

void WorldSystem::create_organ_3(vec2 startingPos) {
	Mix_PlayMusic(organ3_background_music, -1);
	fpsTextEntity = createFpsText();
	currentOrgan = GAME_STATES::ORGAN_3;
	game_state = (unsigned int)GAME_STATES::ORGAN_3;
	createBackground(renderer, TEXTURE_ASSET_ID::ORGAN_3_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	Entity node = createMinigameNode(renderer, vec2(window_width_px / 2 + 400, window_height_px / 2), GAME_STATES::MINIGAME_3);
	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });
	createTransportNode(NODE_POS_3_TO_2, GAME_STATES::ORGAN_2);
	if ((unsigned int)completedOrgan >= (unsigned int)currentOrgan) {
		// Minigame has been beaten here
		createTransportNode(NODE_POS_3_TO_4, GAME_STATES::ORGAN_4);
		registry.colors.insert(node, vec3(0.1f, 1.0f, 0.8f));
	}
	display_inventory();
}

void WorldSystem::create_organ_4(vec2 startingPos) {
	Mix_PlayMusic(organ4_background_music, -1);
	fpsTextEntity = createFpsText();
	currentOrgan = GAME_STATES::ORGAN_4;
	game_state = (unsigned int)GAME_STATES::ORGAN_4;
	createBackground(renderer, TEXTURE_ASSET_ID::ORGAN_4_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	Entity node = createMinigameNode(renderer, vec2(window_width_px / 2 -300, window_height_px / 2 -300), GAME_STATES::MINIGAME_4);
	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });
	createTransportNode(NODE_POS_4_TO_3, GAME_STATES::ORGAN_3);
	if ((unsigned int)completedOrgan >= (unsigned int)currentOrgan) {
		// Minigame has been beaten here
		createTransportNode(NODE_POS_4_TO_5, GAME_STATES::ORGAN_5);
		registry.colors.insert(node, vec3(0.1f, 1.0f, 0.8f));
	}
	display_inventory();
}

void WorldSystem::create_organ_5(vec2 startingPos) {
	Mix_PlayMusic(organ5_background_music, -1);
	fpsTextEntity = createFpsText();
	currentOrgan = GAME_STATES::ORGAN_5;
	game_state = (unsigned int)GAME_STATES::ORGAN_5;
	createBackground(renderer, TEXTURE_ASSET_ID::ORGAN_5_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	Entity node = createMinigameNode(renderer, vec2(window_width_px / 2 + 150, window_height_px / 2 - 100), GAME_STATES::MINIGAME_5);
	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });
	createTransportNode(NODE_POS_5_TO_4, GAME_STATES::ORGAN_4);
	if ((unsigned int)completedOrgan >= (unsigned int)currentOrgan) {
		// Minigame has been beaten here
		createTransportNode(NODE_POS_5_TO_BRAIN, GAME_STATES::BRAIN_LOCKED);
		registry.colors.insert(node, vec3(0.1f, 1.0f, 0.8f));
	}
	display_inventory();
}

// Called brain instead of organ 6 just incase we wanted more organs
void WorldSystem::create_brain(vec2 startingPos) {
	Mix_PlayMusic(brain_background_music, -1);
	fpsTextEntity = createFpsText();

	player = createPlayer(renderer, startingPos);
	genericSpacebarTextBox = createGenericSpaceBarTextBox(renderer, TEXTURE_ASSET_ID::TEXT_BOX, { window_width_px / 2,  window_height_px - 100 });
	enoughItems = check_enough_items();
	if (!brainGateUnlocked) {
		currentOrgan = GAME_STATES::BRAIN_LOCKED;
		game_state = (unsigned int)GAME_STATES::BRAIN_LOCKED;
		createBackground(renderer, TEXTURE_ASSET_ID::BRAIN_LOCKED_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
		brainGateNode = createBrainItemCheckNode(vec2(window_width_px / 2+60, window_height_px / 2 + 160));
	}
	else {
		currentOrgan = GAME_STATES::BRAIN_UNLOCKED;
		game_state = (unsigned int)GAME_STATES::BRAIN_UNLOCKED;
		createBackground(renderer, TEXTURE_ASSET_ID::BRAIN_UNLOCKED_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	}
	Entity killEnding = createBrainEndingChoiceNode(vec2(window_width_px / 2 - 130, window_height_px / 2 - 120));
	registry.brainEndingChoiceNode.get(killEnding).killEnding = 1;
	Entity helpEnding = createBrainEndingChoiceNode(vec2(window_width_px / 2 + 220, window_height_px / 2 - 80));
	registry.brainEndingChoiceNode.get(helpEnding).killEnding = 0;
	createTransportNode(NODE_POS_BRAIN_TO_5, GAME_STATES::ORGAN_5);
	display_inventory();
}

void WorldSystem::create_brain_kill() {
	Mix_PlayMusic(brain_kill_background_music, -1);
	inEndingScene = true;
	createFinalCutSceneAnimation(TEXTURE_ASSET_ID::BRAIN_KILL_ANIMATION, 10, 17,162);
	
}

void WorldSystem::create_brain_help() {
	Mix_PlayMusic(brain_help_background_music, -1);
	inEndingScene = true;
	createFinalCutSceneAnimation(TEXTURE_ASSET_ID::BRAIN_HELP_ANIMATION, 10, 15, 143);
}

bool WorldSystem::check_enough_items() {
	if (mg1_inv_points < MG1_ITEM_GOAL) return false;
	if (mg2_inv_points < MG2_ITEM_GOAL) return false;
	if (mg3_inv_points < MG3_ITEM_GOAL) return false;
	if (mg4_inv_points < MG4_ITEM_GOAL) return false;
	if (mg5_inv_points < MG5_ITEM_GOAL) return false;
	return true;
}

//void WorldSystem::createInitialNodes() {

//	Entity mg3Node = createNode(renderer, { mg3NodeX, mg3NodeY }, GAME_STATES::MINIGAME_3);
//
//	// create hovering arrows
//	Entity leftArrow = createArrow(renderer, { leftNodeX, leftNodeY - 50 },leftNode);
//	Entity rightArrow = createArrow(renderer, { rightNodeX, rightNodeY - 50 }, rightNode);
//	Entity mg3Arrow = createArrow(renderer, { mg3NodeX, mg3NodeY - 50 }, mg3Node);
//
//	// Color the arrows
//	vec3 arrowColor = vec3(0.1, 0.1, 0.4); // TODO: Black for now. Do we want to keep?
//	registry.colors.insert(leftArrow, arrowColor);
//	registry.colors.insert(rightArrow, arrowColor);
//	registry.colors.insert(mg3Arrow, arrowColor);
//
//	// color the nodes
//	vec3 nodeColor = vec3(0.1f, 1.0f, 0.8f);
//	registry.colors.insert(leftNode, nodeColor);
//	registry.colors.insert(rightNode, nodeColor);
//	registry.colors.insert(mg3Node, nodeColor);
//}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;

	if (game_state == (unsigned int)GAME_STATES::ORGAN_1||
		game_state == (unsigned int)GAME_STATES::ORGAN_2||
		game_state == (unsigned int)GAME_STATES::ORGAN_3||
		game_state == (unsigned int)GAME_STATES::ORGAN_4||
		game_state == (unsigned int)GAME_STATES::ORGAN_5||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		)
	{
		// this only works because we should only have Gen/node collisons in the overworld FOR NOW
		if (collisionsRegistry.components.size() == 0) {
			// Not on any node
			registry.overlayRenderRequests.remove(genericSpacebarTextBox);
			registry.remove_all_components_of(genericTextBoxAccompanyingText);
			genericSpacebarTextBoxActive = false;
		}
		else if (registry.transportNodes.has(collisionsRegistry.entities[0]) || registry.transportNodes.has(collisionsRegistry.entities[1])) {
			// On a transport node
			if (!genericSpacebarTextBoxActive) {
				registry.overlayRenderRequests.insert(
					genericSpacebarTextBox,
					{ TEXTURE_ASSET_ID::TEXT_BOX,
						EFFECT_ASSET_ID::TEXTURED,
						GEOMETRY_BUFFER_ID::SPRITE });
				genericSpacebarTextBoxActive = true;

				// Get the correct collision entity (the transport node)
				TransportNode transportNode;
				if (registry.transportNodes.has(collisionsRegistry.entities[0])) {
					transportNode = registry.transportNodes.get(collisionsRegistry.entities[0]);
				}
				else {
					transportNode = registry.transportNodes.get(collisionsRegistry.entities[1]);
				}

				// Depending on the organ, print the accompanying text
				if (transportNode.nextOrgan == (unsigned int)GAME_STATES::ORGAN_1) {
					genericTextBoxAccompanyingText = createText("INTESTINE", vec2(690, 120), 1, vec3(0, 0, 0));
				}
				else if (transportNode.nextOrgan == (unsigned int)GAME_STATES::ORGAN_2) {
					genericTextBoxAccompanyingText = createText("KIDNEY", vec2(725, 120), 1, vec3(0, 0, 0));
				}
				else if (transportNode.nextOrgan == (unsigned int)GAME_STATES::ORGAN_3) {
					genericTextBoxAccompanyingText = createText("STOMACH", vec2(680, 120), 1, vec3(0, 0, 0));
				}
				else if (transportNode.nextOrgan == (unsigned int)GAME_STATES::ORGAN_4) {
					genericTextBoxAccompanyingText = createText("LIVER", vec2(725, 120), 1, vec3(0, 0, 0));
				}
				else if (transportNode.nextOrgan == (unsigned int)GAME_STATES::ORGAN_5) {
					genericTextBoxAccompanyingText = createText("LUNG", vec2(730, 120), 1, vec3(0, 0, 0));
				}
				else if (transportNode.nextOrgan == (unsigned int)GAME_STATES::BRAIN_LOCKED) {
					genericTextBoxAccompanyingText = createText("BRAIN", vec2(730, 120), 1, vec3(0, 0, 0));
				}
			}
		}
		else if (registry.brainItemCheckNode.has(collisionsRegistry.entities[0]) || registry.brainItemCheckNode.has(collisionsRegistry.entities[1])) {
			// On brain check node
			// Do check of items
			if (!brainGateUnlocked) {
				if (!genericSpacebarTextBoxActive) {
					registry.overlayRenderRequests.insert(
						genericSpacebarTextBox,
						{ TEXTURE_ASSET_ID::TEXT_BOX,
							EFFECT_ASSET_ID::TEXTURED,
							GEOMETRY_BUFFER_ID::SPRITE });
					 if (!enoughItems){
						 genericTextBoxAccompanyingText = createText("NOT ENOUGH ITEMS", vec2(575, 120), 1, vec3(0, 0, 0));
						 genericSpacebarTextBoxActive = true;
					 }
					 else {
						 genericTextBoxAccompanyingText = createText("CROSS BLOOD BRAIN BARRIER", vec2(550, 120), 0.75, vec3(0, 0, 0));
						 genericSpacebarTextBoxActive = true;
					 }
					 showGoal = true;
					 display_inventory();
				}
			}
			// }
		}
		else if (registry.brainEndingChoiceNode.has(collisionsRegistry.entities[0]) || registry.brainEndingChoiceNode.has(collisionsRegistry.entities[1])) {
			// On brain ending node
			// Do check of items
			BrainEndingChoiceNode endingNode = registry.brainEndingChoiceNode.has(collisionsRegistry.entities[0]) ? registry.brainEndingChoiceNode.get(collisionsRegistry.entities[0]) : registry.brainEndingChoiceNode.get(collisionsRegistry.entities[1]);
			if (!genericSpacebarTextBoxActive) {
				registry.overlayRenderRequests.insert(
					genericSpacebarTextBox,
					{ TEXTURE_ASSET_ID::TEXT_BOX,
						EFFECT_ASSET_ID::TEXTURED,
						GEOMETRY_BUFFER_ID::SPRITE });
				if (endingNode.killEnding) {
					genericTextBoxAccompanyingText = createText("KILL HUMAN", vec2(680, 120), 1, vec3(0, 0, 0));
				}
				else {
					genericTextBoxAccompanyingText = createText("ENLIGHTEN HUMAN", vec2(590, 120), 1, vec3(0, 0, 0));
				}
				genericSpacebarTextBoxActive = true;
			}
		}
		else if (!genericSpacebarTextBoxActive) {
			// On a minigame node
			Mix_PlayChannel(-1, overworld_on_node_sound, 0);
			registry.overlayRenderRequests.insert(
				genericSpacebarTextBox,
					{ 	TEXTURE_ASSET_ID::TEXT_BOX,
						EFFECT_ASSET_ID::TEXTURED,
						GEOMETRY_BUFFER_ID::SPRITE });
			genericTextBoxAccompanyingText = createText("ENTER GAME", vec2(650, 120), 1, vec3(0, 0, 0));
			genericSpacebarTextBoxActive = true;
		}

	} 
	else if (game_state == (unsigned int)GAME_STATES::MINIGAME_1)
	{
		for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
			// The entity and its collider
			Entity& entity = collisionsRegistry.entities[i];
			Entity& entity_other = collisionsRegistry.components[i].other;

			// Player is collecting items
			if (entity == player_mg && registry.consumables.has(entity_other)) {
				foregroundMotion& atp_motion = registry.foregroundMotions.get(entity_other);

				// Get the InstanceRenderRequest for ATP and remove corresponding offset from translation vector 
				InstanceRenderRequest& irr = registry.instanceRenderRequests.components[0];
				vec2 offset = vec2((atp_motion.position.x - atpStart.x) / (CURRENT_SPRITE_OFFSET * window_width_px / MAP_BLOCK_SIZE), (atp_motion.position.y - atpStart.y) / (CURRENT_SPRITE_OFFSET * window_height_px / MAP_BLOCK_SIZE));
				auto it = std::find(irr.translations.begin(), irr.translations.end(), offset);
				if (it != irr.translations.end())
				{
					irr.translations.erase(it);
					irr.instances--;
				}
				else
				{
					printf("Element not found in translation vector");
				}

				// Remove atp from registry
				if (!registry.instanceRenderRequests.has(entity_other))
					registry.remove_all_components_of(entity_other);
				else
					registry.collidables.remove(entity_other);

				// add points and play music 
				++points;
				Text& scoreText = registry.texts.get(mg1_score);
				scoreText.str = std::to_string(points);
				if (points == 10) scoreText.pos.x -= 5;
				Mix_PlayChannel(-1, mg1_pickup_atp_sound, 0);

				// check if all the consumables are collected -> win!
				if (points == numberOfConsumables) {
					mg1_inv_points += points;
					points = 0;
					std::vector<Entity> entities_to_remove = { entity, registry.deadlys.entities[0], registry.instanceRenderRequests.entities[0]};
					minigame_win_lose_overlay(true, entities_to_remove, TEXTURE_ASSET_ID::MG1_WIN_SHEET, 12);
					if ((unsigned int)completedOrgan < (unsigned int)GAME_STATES::ORGAN_1) {
						completedOrgan = (unsigned int) GAME_STATES::ORGAN_1;
					}
				}
			}
			else if (entity == player_mg && entity_other == enemy_rbc) {
				mg1_inv_points += points;
				points = 0;
				Mix_PlayChannel(-1, mg1_die_sound, 0);
				std::vector<Entity> entities_to_remove = { entity, entity_other };
				minigame_win_lose_overlay(false, entities_to_remove, TEXTURE_ASSET_ID::MG1_LOSE_SHEET, 12);
			}

			// Remove all collisions from this simulation step
			registry.collisions.clear();
		}
	}
	else if (game_state == (unsigned int)GAME_STATES::MINIGAME_2)
	{
		for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
			// The entity and its collider
			Entity& entity = collisionsRegistry.entities[i];
			Entity& entity_other = collisionsRegistry.components[i].other;

			// TODO: properly handle collisions between Gen and Worms 
			if (entity == player_mg && registry.deadlys.has(entity_other)) {
				points = 0;
				mg2_duration = 40000.0f;
				std::vector<Entity> entities_to_remove = { player };
				minigame_win_lose_overlay(false, entities_to_remove, TEXTURE_ASSET_ID::MG2_LOSE_SHEET, 12);
			}

			// Player is collecting items
			if (entity == player_mg && registry.consumables.has(entity_other)) {
				registry.remove_all_components_of(entity_other);
				++points;
				++mg2_inv_points;
				Mix_PlayChannel(-1, mg1_pickup_atp_sound, 0);
			}

			// Remove all collisions from this simulation step
			registry.collisions.clear();
		}
	}
	else if (game_state == (unsigned int)GAME_STATES::MINIGAME_3)
	{
		for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
			// The entity and its collider
			Entity& entity = collisionsRegistry.entities[i];
			Entity& entity_other = collisionsRegistry.components[i].other;

			if (entity == player_mg && registry.deadlys.has(entity_other))
			{
				points = 0;
				std::vector<Entity> entities_to_remove = { entity };
				num_of_glucose = 10;
				minigame_win_lose_overlay(false, entities_to_remove, TEXTURE_ASSET_ID::MG3_LOSE_SHEET, 28);
			}
			else if ((registry.platform.has(entity) || registry.consumables.has(entity)) && registry.deadlys.has(entity_other))
			{
				registry.remove_all_components_of(entity);
			}
			else if (entity == player_mg && registry.consumables.has(entity_other))
			{
				registry.remove_all_components_of(entity_other);
				points++;
				mg3_inv_points++;
				Mix_PlayChannel(-1, mg1_pickup_atp_sound, 0);
			}
			else if (entity == player_mg && registry.finishLine.has(entity_other))
			{
				points = 0;
				num_of_glucose = 10;
				std::vector<Entity> entities_to_remove = { player };
				minigame_win_lose_overlay(true, entities_to_remove, TEXTURE_ASSET_ID::MG3_WIN_SHEET, 14);
				if ((unsigned int)completedOrgan < (unsigned int)GAME_STATES::ORGAN_3) {
					completedOrgan = (unsigned int)GAME_STATES::ORGAN_3;
				}
			}

			// Remove all collisions from this simulation step
			registry.collisions.clear();
		}
	}
	else if (game_state == (unsigned int)GAME_STATES::MINIGAME_4) {
		for (Entity& collisionEntity : registry.collisions.entities) {
			if (registry.consumables.has(collisionEntity) && registry.foregroundRenderRequests.has(collisionEntity)) {
				registry.foregroundRenderRequests.remove(collisionEntity);
				registry.collisions.remove(collisionEntity);
				mg4_inv_points++;
				Mix_PlayChannel(-1, mg1_pickup_atp_sound, 0);
			}
		}
	}
	else if (game_state == (unsigned int)GAME_STATES::MINIGAME_5) {
		for (Entity& collisionEntity : registry.collisions.entities) {

			if (registry.bricks.has(collisionEntity)) {
				Brick& brick = registry.bricks.get(collisionEntity);
				backgroundMotions& motion = registry.backgroundMotions.get(collisionEntity);
				if (brick.hasOxygen) {
					createOxygen(motion.position);
				}
				if (brick.powerUpType != (unsigned int)PowerUpType::NONE) {
					createPowerUp(motion.position, (PowerUpType)brick.powerUpType);
				}
				numBricks--;

				// Generate particles upon brick destruction
				Entity first_particle = createParticle(motion.position, vec3(0.6549, 0.9490, 0.0000));
				std::vector<vec2> particle_translations = generateParticles(motion.position, vec3(0.6549, 0.9490, 0.0000));
				createInstanceRender(renderer, TEXTURE_ASSET_ID::PARTICLE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE, INSTANCING_BUFFER_ID::PARTICLE, first_particle, particle_translations); 
		
				Text& text = registry.texts.get(remainingBricksText);
				std::string str = "Remaining Phlegm: " + std::to_string(numBricks);
				text.str = str;
				registry.remove_all_components_of(collisionEntity);
			}

			if (registry.consumables.has(collisionEntity)) {
				if (registry.powerUps.has(collisionEntity)) {

					PowerUp& powerUp = registry.powerUps.get(collisionEntity);
					
					if (powerUp.type == (unsigned int)PowerUpType::MULTIPLY) {
						std::vector<Entity> ballEntities = registry.balls.entities;
						for (Entity& ballEntity : ballEntities) {
							foregroundMotion& motion = registry.foregroundMotions.get(ballEntity);
							vec2 pos = motion.position;

							float velY = (motion.velocity.y > 0) ? 200.f : -200.f;

							createBall(pos, vec2(-200.f, velY));
							createBall(pos, vec2(200.f, velY));
							Mix_PlayChannel(-1, mg5_x3_multiplier_sound, 0);
						}
					} else if (powerUp.type == (unsigned int)PowerUpType::LONGPADDLE) {
						Paddle& paddle = registry.paddles.components[0];

						if (paddle.longPaddle_ms <= 0.f) {
							Entity& paddleEntity = registry.paddles.entities[0];
							foregroundMotion& motion = registry.foregroundMotions.get(paddleEntity);
							motion.scale.x = 200.f;

							paddle.longPaddle_ms = 6000.f;
							Mix_PlayChannel(-1, mg5_paddle_increase_sound, 0);
						}
					}

					registry.remove_all_components_of(collisionEntity);

				} else { // must be an oxygen
					registry.remove_all_components_of(collisionEntity);
					mg5_inv_points++;
					Mix_PlayChannel(-1, mg1_pickup_atp_sound, 0);
				}
			}

		}
		if (!mg5_gameFinished) {
			if (numBricks == 0) {
				std::vector<Entity> entities_to_remove = { player };
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.bricks.entities), std::end(registry.bricks.entities));
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.consumables.entities), std::end(registry.consumables.entities));
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.balls.entities), std::end(registry.balls.entities));
				minigame_win_lose_overlay(true, entities_to_remove, TEXTURE_ASSET_ID::MG5_WIN_SHEET, 12);
				if ((unsigned int)completedOrgan < (unsigned int)GAME_STATES::ORGAN_5) {
					completedOrgan = (unsigned int)GAME_STATES::ORGAN_5;
				}
				mg5_gameFinished = true;
			} else if (registry.balls.components.empty()) {
				std::vector<Entity> entities_to_remove = { player };
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.bricks.entities), std::end(registry.bricks.entities));
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.consumables.entities), std::end(registry.consumables.entities));
				entities_to_remove.insert(std::end(entities_to_remove), std::begin(registry.balls.entities), std::end(registry.balls.entities));
				minigame_win_lose_overlay(false, entities_to_remove, TEXTURE_ASSET_ID::MG5_LOSE_SHEET, 20);
				mg5_gameFinished = true;
			}
		}
		registry.collisions.clear();
	}
}

void WorldSystem::change_game_states(enum GAME_STATES game) {
	game_state_system.newGameStateContainers();

	// Jumps to the different minigames
	switch (game) {
		case GAME_STATES::MINIGAME_1:
			create_minigame_1();
			break;
		case GAME_STATES::MINIGAME_2:
			create_minigame_2();
			break;
		case GAME_STATES::MINIGAME_3:
			create_minigame_3();
			break;
		case GAME_STATES::MINIGAME_4:
			create_minigame_4();
			break;
		case GAME_STATES::MINIGAME_5:
			create_minigame_5();
			break;
		case GAME_STATES::ORGAN_1:
			create_organ_1(player_saved_pos);
			break;
		case GAME_STATES::ORGAN_2:
			create_organ_2(player_saved_pos);
			break;
		case GAME_STATES::ORGAN_3:
			create_organ_3(player_saved_pos);
			break;
		case GAME_STATES::ORGAN_4:
			create_organ_4(player_saved_pos);
			break;
		case GAME_STATES::ORGAN_5:
			create_organ_5(player_saved_pos);
			break;
		case GAME_STATES::BRAIN_LOCKED: {} // Same logic for both game changes
		case GAME_STATES::BRAIN_UNLOCKED:
			create_brain(player_saved_pos);
			break;
		case GAME_STATES::BRAIN_KILL:
			create_brain_kill();
			break;
		case GAME_STATES::BRAIN_HELP:
			create_brain_help();
			break;
		case GAME_STATES::TITLE:
			create_title();
			break;
		case GAME_STATES::CREDITS:
			create_credits();
			break;
	}
}

// Pacman
void WorldSystem::create_minigame_1() {
	fpsTextEntity = createFpsText();
	Mix_PlayMusic(mg1_background_music, -1);
	game_state = (unsigned int)GAME_STATES::MINIGAME_1;
	createText("ATPS:", {window_width_px - 105, window_height_px - 45}, 0.75, {0,0,0});
	mg1_score = createText(std::to_string(points), {window_width_px-73, window_height_px - 85}, 0.75, {0,0,0});
	minigame1_maze_gen();
	vec2 counterPosition = { window_width_px - 60, 50 };
	vec2 playerStart = { CURRENT_SPRITE_OFFSET , CURRENT_SPRITE_OFFSET };
	vec2 enemyStart = { window_width_px - CURRENT_SPRITE_OFFSET, window_height_px - CURRENT_SPRITE_OFFSET };
	Entity backgroundEntity = createBackground(renderer, TEXTURE_ASSET_ID::MINIGAME_1, { window_width_px / 2, window_height_px / 2 }, { window_width_px/2, -window_height_px/2});
	if (registry.players.entities.size() <= 1) {
		player_mg = createPlayer(renderer, playerStart);
		registry.foregroundMotions.get(player_mg).scale = { 80,-80 };
	}
	else {
		registry.foregroundMotions.get(player_mg).position = playerStart;
		registry.foregroundRenderRequests.insert(
			player_mg,
			{ TEXTURE_ASSET_ID::GEN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	if (registry.deadlys.entities.size() < 1) {
		enemy_rbc = createRedBloodCell(enemyStart);
	}
	else {
		registry.foregroundMotions.get(enemy_rbc).position = enemyStart;
		registry.foregroundRenderRequests.insert(
			enemy_rbc,
			{ TEXTURE_ASSET_ID::RBC_SHEET,
				EFFECT_ASSET_ID::ANIMATION,
				GEOMETRY_BUFFER_ID::SPRITE });
	}


	float yPos = CURRENT_SPRITE_OFFSET;
	numberOfConsumables = 0;
	std::vector<vec2> translations;
	for (int y = 0; y < 9; y++) {
		float xPos = CURRENT_SPRITE_OFFSET;
		for (int x = 0; x < 16; x++) {

			if (GAME_MAZE[y][x] && !(xPos == counterPosition.x + 10 && yPos == counterPosition.y)) {
				createWall(renderer, vec2(xPos, yPos), {100,-100});
			}
			else if (!GAME_MAZE[y][x] && !((xPos == playerStart.x && yPos == playerStart.y) || (xPos == enemyStart.x && yPos == enemyStart.y)))
			{
				createItem_ATP(renderer, vec2(xPos, yPos));
				vec2 offset = vec2((xPos - atpStart.x) / (CURRENT_SPRITE_OFFSET * window_width_px / MAP_BLOCK_SIZE), (yPos - atpStart.y) / (CURRENT_SPRITE_OFFSET * window_height_px / MAP_BLOCK_SIZE));
				translations.push_back(offset);
				numberOfConsumables++;
			}
			xPos += MAP_BLOCK_SIZE;
		}
		yPos += MAP_BLOCK_SIZE;
	}

	createInstanceRender(renderer, TEXTURE_ASSET_ID::ITEM_ATP, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE, INSTANCING_BUFFER_ID::ATP, registry.consumables.entities[0], translations); 
	
	// tutorial stuff
	if (!tutorialChecklist[GAME_STATES::MINIGAME_1]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MG1_TUTORIAL);
		tutorialChecklist[GAME_STATES::MINIGAME_1] = true;
	}
	createBackground(renderer, TEXTURE_ASSET_ID::BLANK_OVERLAY, counterPosition, { 65, 50 });
}

void WorldSystem::minigame1_maze_gen() {
	// Reset game maze to initial seed
	for (int j = 0; j < 9; j++) {
		for (int i = 0; i < 16; i++) {
			GAME_MAZE[j][i] = GAME_MAZE_SEED[j][i];
		}
	}
	std::vector<vec2> visited = {};
	vec2 pos = { (rand() % 8) * 2, (rand() % 8) * 2 };
	minigame1_carve_maze(pos, visited);
	GAME_MAZE[0][0] = 0; GAME_MAZE[8][15] = 0;
}

void WorldSystem::minigame1_carve_maze(vec2 pos, std::vector<vec2>& visited) {
	// DFS Recursive search until all nodes are visited
	if ((count(visited.begin(), visited.end(), pos) != 0)) return;
	visited.push_back(pos);
	GAME_MAZE[(int)pos.y][(int)pos.x] = 0;
	std::vector<Direction> possibleDirections = { };
	if (pos.y - 2 >= 0) possibleDirections.push_back(UP);
	if (pos.y + 2 <= 8) possibleDirections.push_back(DOWN);
	if (pos.x - 2 >= 0) possibleDirections.push_back(LEFT);
	if (pos.x + 2 <= 15) possibleDirections.push_back(RIGHT);
	while (possibleDirections.size() > 0) {
		int randomIndex = rand() % possibleDirections.size();
		vec2 newPos = { 0,0 };
		switch (possibleDirections[randomIndex]) {
		case UP:
			newPos = { pos.x, pos.y-2 };
			if (count(visited.begin(), visited.end(), newPos) == 0) {
				GAME_MAZE[(int)pos.y - 1][(int)pos.x] = 0;
				minigame1_carve_maze({ pos.x, pos.y - 2 }, visited);
			}
			break;
		case RIGHT:
			newPos = { pos.x + 2, pos.y };
			if (count(visited.begin(), visited.end(), newPos) == 0) {
				GAME_MAZE[(int)pos.y][(int)pos.x + 1] = 0;
				minigame1_carve_maze({ pos.x + 2, pos.y }, visited);
			}
			break;
		case DOWN:
			newPos = { pos.x, pos.y + 2 };
			if (count(visited.begin(), visited.end(), newPos) == 0) {
				GAME_MAZE[(int)pos.y + 1][(int)pos.x] = 0;
				minigame1_carve_maze({ pos.x, pos.y + 2 }, visited);
			}
			break;
		case LEFT:
			newPos = { pos.x - 2, pos.y };
			if (count(visited.begin(), visited.end(), newPos) == 0) {
				GAME_MAZE[(int)pos.y][(int)pos.x - 1] = 0;
				minigame1_carve_maze({ pos.x - 2, pos.y }, visited);
			}
			break;
		}
		possibleDirections.erase(possibleDirections.begin() + randomIndex);
	}
}

// Falling / Moving through blood vessel
void WorldSystem::create_minigame_2() {
	fpsTextEntity = createFpsText();
	Mix_PlayMusic(mg2_background_music, -1);
	Entity backgroundEntity_1 = createBackground(renderer, TEXTURE_ASSET_ID::MINIGAME_2, { 0, window_height_px / 2 }, { window_width_px / 2 + 10 , -window_height_px / 2 });
	Entity backgroundEntity_2 = createBackground(renderer, TEXTURE_ASSET_ID::MINIGAME_2, { window_width_px, window_height_px / 2 }, { window_width_px / 2 + 10 , -window_height_px / 2 });
	numberOfConsumables = 15;
	float x_pos = window_width_px;

	player_mg = createPlayer(renderer, { 100, window_height_px/2 });
	createText("   Progress " , { 25, window_height_px - 55 }, 1, { 0,0,0});
	createMG2ProgressBar({ window_width_px / 2 + 150, 42 }, { 1200, 50});
	createMG2Progress({ 410, 39 }, { 32,30 });
	
	for (int i = 0; i < numberOfConsumables; i++) {
		float rand = uniform_dist(rng);
		x_pos += rand*window_width_px + 150;
		float y_pos = rand > 0.5f ? 170.f : 750.f;
		createItem_Lipid(renderer, { x_pos, y_pos }, 2.5f-rand);
	}
	initMG2Physics();
	initializeMG2Tree(renderer);
	game_state = (unsigned int)GAME_STATES::MINIGAME_2;
	
	// tutorial stuff
	if (!tutorialChecklist[GAME_STATES::MINIGAME_2]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MG2_TUTORIAL);
		tutorialChecklist[GAME_STATES::MINIGAME_2] = true;
	}
}

void WorldSystem::end_minigame_2(float elapsed_ms_since_last_update) {
	if (game_state == (unsigned int)GAME_STATES::MINIGAME_2) {
		Entity bar = registry.bar.entities.back();
		overlayMotions& bar_motion = registry.overlayMotions.get(bar);

		if (!pause_game_state) {
			mg2_duration -= elapsed_ms_since_last_update;
			bar_motion.scale.x += (1075.f / 40000.f) * elapsed_ms_since_last_update; // 1075 is the width of the progress bar
			bar_motion.position.x += (1075.f / 40000.f) * elapsed_ms_since_last_update / 2;
		}
		if (mg2_duration <= 0 && !pause_game_state) {
			mg2_duration = 40000.0f;
			points = 0;
			std::vector<Entity> entities_to_remove = { player };
			minigame_win_lose_overlay(true, entities_to_remove, TEXTURE_ASSET_ID::MG2_WIN_SHEET, 12);
			if ((unsigned int)completedOrgan < (unsigned int)GAME_STATES::ORGAN_2) {
				completedOrgan = (unsigned int)GAME_STATES::ORGAN_2;
			}
		}
	}
}

// Acid jump
void WorldSystem::create_minigame_3()
{
	jump_count = 0;
	fpsTextEntity = createFpsText();
	Mix_PlayMusic(mg3_background_music, -1);
	Entity backgroundEntity_1 = createBackground(renderer, TEXTURE_ASSET_ID::MG3_BACKGROUND, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });

	player_mg = createPlayer(renderer, { 75, window_height_px / 4 });
	foregroundMotion& motion = registry.foregroundMotions.get(player_mg);
	vec2 starting_pos = motion.position - vec2(0, (motion.scale.y / 2) - 50);
	Entity platform_entity = createPlatform(starting_pos, vec2(150, -50), gravity_factor);
	Platform& platform = registry.platform.get(platform_entity);
	platform.below = true;

	platform_pos.push_back(starting_pos);
	int x = 1;
	int x2 = 5;
	bool right_dir = true;

	for (int i = 1; i < 15; i++)
	{
		vec2 new_plat = starting_pos + vec2(50 * 6 * x, -50 * 3 * i);
		vec2 new_plat_2 = starting_pos + vec2(50 * 6 * x2, -50 * 3 * i);
		if (new_plat.x > window_width_px)
		{
			right_dir = false;
			x -= 2;
			x2 += 2;
			new_plat.x = starting_pos.x + (50 * 6 * x);
			new_plat_2.x = starting_pos.x + (50 * 6 * x2);
		}
		else if (new_plat.x < 0)
		{
			right_dir = true;
			x += 2;
			x2 -= 2;
			new_plat.x = starting_pos.x + (50 * 6 * x);
			new_plat_2.x = starting_pos.x + (50 * 6 * x2);
		}
		
		if (right_dir)
		{
			x++;
			x2--;
		}
		else
		{
			x--;
			x2++;
		}
		
		Entity platform_entity_1 = createPlatform(new_plat, vec2(150, -50), gravity_factor);
		if (new_plat != new_plat_2)
			Entity platform_entity_2 = createPlatform(new_plat_2, vec2(150, -50), gravity_factor);

		platform_pos.push_back(new_plat);
	}

	vec2 last_platform_pos = platform_pos.back();
	Entity finish_line = createFinishLine(vec2(window_width_px / 2, last_platform_pos.y - 100), gravity_factor);

	if (!tutorialChecklist[GAME_STATES::MINIGAME_3]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MG3_TUTORIAL);
		tutorialChecklist[GAME_STATES::MINIGAME_3] = true;
	}

	Entity acid_entity = createAcid(vec2(window_width_px / 2, window_height_px - 50));
	game_state = (unsigned int)GAME_STATES::MINIGAME_3;
}

void WorldSystem::minigame3_step(float elapsed_ms_since_last_update)
{
	if (game_state == (unsigned int)GAME_STATES::MINIGAME_3 && !pause_game_state)
	{
		if (num_of_glucose > 0)
		{
			glucose_counter -= elapsed_ms_since_last_update;
			if (glucose_counter < 0)
			{
				float index = rand() % platform_pos.size();
				Entity glucose = createGlucose(vec2(platform_pos[index].x, -50), gravity_factor * 3);
				num_of_glucose--;
				glucose_counter = 2000.f;
			}
		}
	}
}

// Whack a mole
void WorldSystem::create_minigame_4() {
	fpsTextEntity = createFpsText();
	Mix_PlayMusic(mg4_background_music, -1);
	Entity backgroundEntity_1 = createBackground(renderer, TEXTURE_ASSET_ID::MINIGAME_4, { window_width_px/2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });
	explosionCount = 0;
	for (unsigned int i = 0; i < 7; i++) {
		for (unsigned int j = 0; j < 5; j++) {
			if (i != 0 && i != 6 && j != 0 && j != 4) {
				Entity mole = createWhackAMole(renderer, { i*window_width_px / 6, j*window_height_px / 4 + 100}); // Virus is to move up and down
			}
		}
	}

	createMg4BackgroundOverlay(renderer, TEXTURE_ASSET_ID::MINIGAME_4_OVERLAY, { window_width_px / 2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });

	player_mg = createPlayer(renderer, { 100, window_height_px / 2 });

	game_state = (unsigned int)GAME_STATES::MINIGAME_4;

	for (int i = 0; i < NUM_IRON; i++) createIron();

	// tutorial stuff
	if (!tutorialChecklist[GAME_STATES::MINIGAME_4]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MG4_TUTORIAL);
		tutorialChecklist[GAME_STATES::MINIGAME_4] = true;
	}

	remainingText = createText("REMAINING: 20", { 650, 65 }, 1, { 1, 1, 1 });
}

void WorldSystem::minigame4_step(float elapsed_ms_since_last_update) {
	if (game_state == (unsigned int)GAME_STATES::MINIGAME_4) {
		std::vector<Entity>& whackAMoleEntities = registry.whackAMole.entities;
		for (unsigned int i = 0; i < whackAMoleEntities.size(); i++) {
			WhackAMole& whackAMoleComponent = registry.whackAMole.get(whackAMoleEntities[i]);
			if (whackAMoleComponent.active && !whackAMoleComponent.whacked && !whackAMoleComponent.exploded) {
				if (whackAMoleComponent.angerLevel < ALLOWED_TIME_TO_BE_ALIVE) {
					whackAMoleComponent.angerLevel += elapsed_ms_since_last_update / 7500;
				}
				else {
					Mix_PlayChannel(-1, mg3_explosion_sound, 0);
					explosionCount++;
					whackAMoleComponent.exploded = 1;
					whackAMoleComponent.whacked = 0;
					whackAMoleComponent.angerLevel = 0;
					if (registry.collidables.has(whackAMoleEntities[i])) {
						registry.collidables.remove(whackAMoleEntities[i]);
					}
					registry.foregroundRenderRequests.get(whackAMoleEntities[i]).used_texture = TEXTURE_ASSET_ID::MG4_EXPLOSION;
				}
			}
		}
	}
}

void WorldSystem::end_minigame_4() {
	if (game_state == (unsigned int)GAME_STATES::MINIGAME_4) {
		if (explosionCount >= 3 && !pause_game_state) {
			std::vector<Entity> toRemove = { player_mg};
			toRemove.insert(std::end(toRemove), std::begin(registry.whackAMole.entities), std::end(registry.whackAMole.entities));
			toRemove.insert(std::end(toRemove), std::begin(registry.consumables.entities), std::end(registry.consumables.entities));
			minigame_win_lose_overlay(false, toRemove, TEXTURE_ASSET_ID::MG4_LOSE_SHEET, 11);
			registry.remove_all_components_of(remainingText); //Cleanup
		}
	}
}

// Brick break
void WorldSystem::create_minigame_5() {
	Mix_PlayMusic(mg5_background_music, -1);
	fpsTextEntity = createFpsText();
	createBackground(renderer, TEXTURE_ASSET_ID::MG5_BACKGROUND, { window_width_px/2, window_height_px / 2 }, { window_width_px / 2 , -window_height_px / 2 });

	createBall(vec2(window_width_px / 2, window_height_px - 100.0f), vec2(0.f, -350.f));
	player = createPaddle();
	mg5_gameFinished = false;

	float yPos = BRICK_HEIGHT;
	numBricks = 0;
	const int numMaps = sizeof(BRICK_MAP_ARRAY) / sizeof(BRICK_MAP_ARRAY[0]);
	const int randomSelection = rand() % numMaps;
	const auto brickMap = BRICK_MAP_ARRAY[randomSelection];
	for (int y = 0; y < 18; y++) {
		float xPos = BRICK_WIDTH / 2;
		for (int x = 0; x < 16; x++) {
			
			if (brickMap[y][x]) {
				bool hasOxygen = rand() % 10 == 0;
				PowerUpType powerUp;
				int random = rand();
				if (random % 20 == 0) powerUp = PowerUpType::MULTIPLY;
				else if (random % 15 == 0) powerUp = PowerUpType::LONGPADDLE;
				else powerUp = PowerUpType::NONE;
				
				createBrick(vec2(xPos, yPos), {BRICK_WIDTH, BRICK_HEIGHT}, hasOxygen, powerUp);
				numBricks++;
			}
			xPos += BRICK_WIDTH;
		}
		yPos += BRICK_HEIGHT;
	}

	std::string numBricksStr = "Remaining Phlegm: " + std::to_string(numBricks);
	remainingBricksText = createText(numBricksStr, vec2(window_width_px - 400, 10.0f), 0.75f, vec3(1.0f, 1.0f, 1.0f));

	game_state = (unsigned int)GAME_STATES::MINIGAME_5;

	// tutorial stuff
	if (!tutorialChecklist[GAME_STATES::MINIGAME_5]) {
		pause_game_state = true;
		tutorial = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MG5_TUTORIAL);
		tutorialChecklist[GAME_STATES::MINIGAME_5] = true;
	}
}

void WorldSystem::minigame5_step(float elapsed_ms_since_last_update) {
	Paddle& paddle = registry.paddles.components[0];

	if (paddle.longPaddle_ms > 0.f) {
		paddle.longPaddle_ms -= elapsed_ms_since_last_update;
	} else {
		Entity& paddleEntity = registry.paddles.entities[0];
		foregroundMotion& motion = registry.foregroundMotions.get(paddleEntity);

		motion.scale.x = 150.f;
	}
}

void WorldSystem::display_inventory() {
	float fontSize = 0.40;
	int textStartingY = window_height_px - 40;
	int iconStartingY = 75;
	float spacingX = 20;
	float spacingY = 40;
	vec2 defaultScale = { 15,-15 };

	createBackground(renderer, TEXTURE_ASSET_ID::BLANK_OVERLAY, { 90,140 }, { 100, 140 });
	createText("INVENTORY", { 25, textStartingY }, 0.5, { 0,0,0 });

	std::string mg1_inventory_text = "    x" + std::to_string(mg1_inv_points);
	Entity atp_icon = createBackground(renderer, TEXTURE_ASSET_ID::ITEM_ATP, { 25 + spacingX, iconStartingY }, defaultScale);
	if (!mg1_inv_points) {
		mg1_inventory_text = "    ???";
		registry.colors.emplace(atp_icon) = { 0,0,0 };
	}
	if (showGoal) {
		mg1_inventory_text = mg1_inventory_text + "/" + std::to_string(MG1_ITEM_GOAL);
	}
	createText(mg1_inventory_text, { 35 + spacingX, textStartingY - spacingY }, fontSize, { 0,0,0 });

	std::string mg2_inventory_text = "    x" + std::to_string(mg2_inv_points);
	Entity lipid_inventory_icon = createBackground(renderer, TEXTURE_ASSET_ID::ITEM_LIPID, { 25 + spacingX, iconStartingY + spacingY * 1 }, { 20, -12 });
	if (!mg2_inv_points) {
		mg2_inventory_text = "    ???";
		registry.colors.emplace(lipid_inventory_icon) = { 0,0,0 };
	}
	if (showGoal) {
		mg2_inventory_text = mg2_inventory_text + "/" + std::to_string(MG2_ITEM_GOAL);
	}
	createText(mg2_inventory_text, { 35 + spacingX, textStartingY - spacingY * 2 }, fontSize, { 0,0,0 });

	std::string mg3_inventory_text = "    x" + std::to_string(mg3_inv_points);
	Entity gluce_inventory_icon = createBackground(renderer, TEXTURE_ASSET_ID::ITEM_GLUCOSE, { 25 + spacingX, iconStartingY + spacingY * 2 }, { 20,-20 });
	if (!mg3_inv_points) {
		mg3_inventory_text = "    ???";
		registry.colors.emplace(gluce_inventory_icon) = { 0,0,0 };
	}
	if (showGoal) {
		mg3_inventory_text = mg3_inventory_text + "/" + std::to_string(MG3_ITEM_GOAL);
	}
	createText(mg3_inventory_text, { 35 + spacingX, textStartingY - spacingY * 3 }, fontSize, { 0,0,0 });

	std::string mg4_inventory_text = "    x" + std::to_string(mg4_inv_points);
	Entity wbc_inventory_icon = createBackground(renderer, TEXTURE_ASSET_ID::IRON, { 25 + spacingX, iconStartingY + spacingY * 3 }, defaultScale);
	if (!mg4_inv_points) {
		mg4_inventory_text = "    ???";
		registry.colors.emplace(wbc_inventory_icon) = { 0,0,0 };
	}
	if (showGoal) {
		mg4_inventory_text = mg4_inventory_text + "/" + std::to_string(MG4_ITEM_GOAL);
	}
	createText(mg4_inventory_text, { 35 + spacingX, textStartingY - spacingY * 4 }, fontSize, { 0,0,0 });

	std::string mg5_inventory_text = "    x" + std::to_string(mg5_inv_points);
	Entity oxygen_inventory_icon = createBackground(renderer, TEXTURE_ASSET_ID::OXYGEN, { 25 + spacingX, iconStartingY + spacingY * 4 }, defaultScale);
	if (!mg5_inv_points) {
		mg5_inventory_text = "    ???";
		registry.colors.emplace(oxygen_inventory_icon) = { 0,0,0 };
	}
	if (showGoal) {
		mg5_inventory_text = mg5_inventory_text + "/" + std::to_string(MG5_ITEM_GOAL);
	}
	createText(mg5_inventory_text, { 35 + spacingX, textStartingY - spacingY * 5 }, fontSize, { 0,0,0 });
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::minigame_win_lose_overlay(bool win, std::vector<Entity>& entities_to_remove, TEXTURE_ASSET_ID used_texture, int num_frames)
{
	for (Entity& entity : entities_to_remove)
	{
		registry.remove_all_components_of(entity);
	}

	if (win)
		minigameWin = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MINIGAME_WIN);
	else
		minigameOver = createTutorial(tutorialPos, TEXTURE_ASSET_ID::MINIGAME_OVER);

	Entity level_cleared_animation = createAnimation(tutorialPos, used_texture, num_frames, { 650, 300 });
	// Entity level_cleared_animation = createAnimation(renderer, tutorialPos, first_frame, last_frame);
	pause_game_state = true;
}

void WorldSystem::load_game_saveFile() {
	game_state_system.read_save_state();
	currentOrgan = game_state_system.saveStatePersistence["currentOrgan"];
	completedOrgan = game_state_system.saveStatePersistence["unlockedOrgan"];
	player_saved_pos = vec2(game_state_system.saveStatePersistence["currentPositionX"], game_state_system.saveStatePersistence["currentPositionY"]);
	tutorialChecklist = game_state_system.saveStatePersistence.at("seenTutorials").get<std::unordered_map<GAME_STATES, bool>>();
	mg1_inv_points = game_state_system.saveStatePersistence["items"]["atp"];
	mg2_inv_points = game_state_system.saveStatePersistence["items"]["lipids"];
	mg3_inv_points = game_state_system.saveStatePersistence["items"]["glucose"];
	mg4_inv_points = game_state_system.saveStatePersistence["items"]["iron"];
	mg5_inv_points = game_state_system.saveStatePersistence["items"]["oxygen"];
	change_game_states(currentOrgan);
}

void WorldSystem::save_game_saveFile(std::string message) {
	game_state_system.saveStatePersistence["currentPositionX"] = registry.foregroundMotions.get(player).position.x;
	game_state_system.saveStatePersistence["currentPositionY"] = registry.foregroundMotions.get(player).position.y;
	game_state_system.saveStatePersistence["currentOrgan"] = currentOrgan;
	game_state_system.saveStatePersistence["unlockedOrgan"] = completedOrgan;
	game_state_system.saveStatePersistence["seenTutorials"] = tutorialChecklist;
	game_state_system.saveStatePersistence["items"]["atp"] = mg1_inv_points;
	game_state_system.saveStatePersistence["items"]["lipids"] = mg2_inv_points;
	game_state_system.saveStatePersistence["items"]["glucose"] = mg3_inv_points;
	game_state_system.saveStatePersistence["items"]["iron"] = mg4_inv_points;
	game_state_system.saveStatePersistence["items"]["oxygen"] = mg5_inv_points;
	Entity savedText = createText(message, { 10, 10 }, 1, { 1, 1, 1 });
	registry.savedGameTimer.emplace(savedText);
	game_state_system.write_save_state();
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// TODO: Make key presses, specific to the specified game states
	// Motion controls
	if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
		// Global mapped keys
		if (action == GLFW_PRESS) keymap[key] = true;
		else if (action == GLFW_RELEASE) keymap[key] = false;

	}

	// W/S specific to title
	if (!pause_game_state && game_state == (unsigned int)GAME_STATES::TITLE) {
		if ((key == GLFW_KEY_W || key == GLFW_KEY_S) && action == GLFW_RELEASE) {
			if (key == GLFW_KEY_W) {
				registry.title.components[0].selectionOption = registry.title.components[0].selectionOption == 0 ? 0 : registry.title.components[0].selectionOption - 1;
			}
			else if (key == GLFW_KEY_S) {
				registry.title.components[0].selectionOption = registry.title.components[0].selectionOption == 2 ? 2 : registry.title.components[0].selectionOption + 1;
			}
			if (registry.title.components[0].titleInPlace) {
				Mix_PlayChannel(-1, title_select_sound, 0);
			}
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		if (game_state == (unsigned int)GAME_STATES::MINIGAME_3 && !pause_game_state) {
			if (jump_count < 2)
			{
				foregroundMotion& player_motion = registry.foregroundMotions.get(player_mg);
				if (!registry.jump.has(player_mg))
				{
					Jump& jump = registry.jump.emplace(player_mg);
				}
				
				player_motion.velocity.y = -175;
				jump_count++;
				on_platform = false;
			}
		}
	}

	
	// Action key
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		if (pause_game_state && registry.overlayMotions.has(tutorial)) {
			registry.overlayMotions.remove(tutorial);
			registry.overlayRenderRequests.remove(tutorial);
			pause_game_state = false;
		}

		if (registry.overlayMotions.has(minigameWin) || registry.overlayMotions.has(minigameOver)) {
			pause_game_state = false;
			change_game_states(currentOrgan);
			save_game_saveFile("AUTO SAVED");
			points = 0;
		}

		if (endingSceneFinished && !fadeOut) {
			fadeOut = true;
			pause_game_state = true;
		}

		if (game_state == (unsigned int)GAME_STATES::CREDITS && !fadeOut) {
			if (registry.credits.components[0].creditsFinished) {
				fadeOut = true;
				pause_game_state = true;
			}
		}

		if (game_state == (unsigned int)GAME_STATES::TITLE && registry.title.components[0].titleInPlace) {
			Mix_PlayChannel(-1, title_enter_sound, 0);
			fadeOut = true;
			pause_game_state = true;
		}
		else if (game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
			game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
			game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
			game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
			game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
			game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
			game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
			&& !fadeOut
			) {
			auto& collisionsRegistry = registry.collisions;
			for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
				// The entity and its collider
				Entity entity = collisionsRegistry.entities[i];
				Entity entity_other = collisionsRegistry.components[i].other;

				// Enter minigame actions
				if (registry.gameNodes.has(entity_other))
				{
					Mix_PlayChannel(-1, title_enter_sound, 0);
					fadeOut = true;
					pause_game_state = true;
				}
				// Transport node action
				else if (registry.transportNodes.has(entity_other)) {
					Mix_PlayChannel(-1, title_enter_sound, 0);
					fadeOut = true;
					pause_game_state = true;
				}
				else if (registry.brainItemCheckNode.has(entity_other) && !brainGateUnlocked && enoughItems) {
					brainGateUnlocked = true;
					registry.overlayRenderRequests.remove(genericSpacebarTextBox);
					registry.remove_all_components_of(genericTextBoxAccompanyingText);
					genericSpacebarTextBoxActive = false;
					player_saved_pos = registry.foregroundMotions.get(player).position;
					change_game_states(GAME_STATES::BRAIN_UNLOCKED);
				}
				else if (registry.brainEndingChoiceNode.has(entity_other)) {
					Mix_PlayChannel(-1, title_enter_sound, 0);
					fadeOut = true;
					pause_game_state = true;
				}
			}
		}
		else if (game_state == (unsigned int)GAME_STATES::MINIGAME_4) {
			auto& collisionsRegistry = registry.collisions;
			for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
				// The entity and its collider
				Entity& entity = collisionsRegistry.entities[i];
				Entity& entity_other = collisionsRegistry.components[i].other;
				if (registry.whackAMole.has(entity_other)) {
					points++;
					registry.texts.get(remainingText).str = "REMAINING: " + std::to_string(20-points);
					Mix_PlayChannel(-1, mg3_whack_sound, 0);
					registry.whackAMole.get(entity_other).whacked = true;
					registry.whackAMole.get(entity_other).angerLevel = 0;
					registry.collidables.remove(entity_other);
					registry.collisions.clear();
				}
				if (points >= 20) {
					std::vector<Entity> toRemove = { player_mg };
					toRemove.insert(std::end(toRemove), std::begin(registry.whackAMole.entities), std::end(registry.whackAMole.entities));
					toRemove.insert(std::end(toRemove), std::begin(registry.consumables.entities), std::end(registry.consumables.entities));
					minigame_win_lose_overlay(true, toRemove, TEXTURE_ASSET_ID::MG4_WIN_SHEET, 12);
					if ((unsigned int)completedOrgan < (unsigned int)GAME_STATES::ORGAN_4) {
						completedOrgan = (unsigned int) GAME_STATES::ORGAN_4;
					}
					registry.remove_all_components_of(remainingText); //Cleanup
					points = 0;
				}
			}
		}
		
	}

	

	// Exit game
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, true);
	}

	// Increase completed organ number
	if ((mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_EQUAL && action == GLFW_RELEASE && (
		game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		)) {
		if (pause_game_state) pause_game_state = false;
		completedOrgan = (unsigned int)currentOrgan;
		change_game_states(currentOrgan);
	}

	// Give all required items
	if ((mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_MINUS && action == GLFW_RELEASE && (
		game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		)) {
		mg1_inv_points = MG1_ITEM_GOAL;
		mg2_inv_points = MG2_ITEM_GOAL;
		mg3_inv_points = MG3_ITEM_GOAL;
		mg4_inv_points = MG4_ITEM_GOAL;
		mg5_inv_points = MG5_ITEM_GOAL;
		player_saved_pos = registry.foregroundMotions.get(player).position;
		change_game_states(currentOrgan);
	}


	// Debugging
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		debugging.in_debug_mode = !debugging.in_debug_mode;
	}

	//// Accessing dev mode removed to avoid player mispresses
	//if (key == GLFW_KEY_V && action == GLFW_RELEASE) {
	//	debugging.in_dev_mode = !debugging.in_dev_mode;
	//	if (debugging.in_dev_mode) {
	//		printf("Turned on DEV mode\n");
	//	}
	//	else {
	//		printf("Turned off DEV mode\n");
	//	}
	//}

	if (debugging.in_dev_mode && (
		game_state == (unsigned int)GAME_STATES::TITLE||
		game_state == (unsigned int)GAME_STATES::ORGAN_1||
		game_state == (unsigned int)GAME_STATES::ORGAN_2||
		game_state == (unsigned int)GAME_STATES::ORGAN_3||
		game_state == (unsigned int)GAME_STATES::ORGAN_4||
		game_state == (unsigned int)GAME_STATES::ORGAN_5||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		)) {
		// Jump to organ 1
		if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_1_TO_2;;
			change_game_states(GAME_STATES::ORGAN_1);
		}

		// Jump to organ 2
		if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_2_TO_1;
			change_game_states(GAME_STATES::ORGAN_2);
		}

		// Jump to organ 3
		if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_3_TO_2;
			change_game_states(GAME_STATES::ORGAN_3);
		}

		if (key == GLFW_KEY_4 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_4_TO_3;
			change_game_states(GAME_STATES::ORGAN_4);
		}

		if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_5_TO_4;
			change_game_states(GAME_STATES::ORGAN_5);
		}

		if (key == GLFW_KEY_6 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_BRAIN_TO_5;
			change_game_states(GAME_STATES::BRAIN_LOCKED); // Note that this goes to brain locked or unlocked.  It will always be locked at first. May add debug mode later for item bypass
		}

		// Increase completed organ number
		if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE) {
			completedOrgan+=2; // Increase by 2 because theres always a minigame state then the organ state
			printf("Completed Organ Game State increased to: %i\n", completedOrgan);
		}

		if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
			mg1_inv_points = MG1_ITEM_GOAL;
			mg2_inv_points = MG2_ITEM_GOAL;
			mg3_inv_points = MG3_ITEM_GOAL;
			mg4_inv_points = MG4_ITEM_GOAL;
			mg5_inv_points = MG5_ITEM_GOAL;
			player_saved_pos = registry.foregroundMotions.get(player).position;
			change_game_states(currentOrgan);
		}

		//Debug printing.  Draw the walls, then press O to print to console.
		if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
			printf("\n");
			for (int i = 0; i < 36; ++i) {
				printf("{");
				for (int j = 0; j < 64; ++j) {
					printf("%d", debugArray[i][j]);
					if (j != 63) printf(", ");
				}
				printf("},\n");
			}
			printf("Done\n");
		}

		//Print existing organ array for debugging. Commented out 
		if (key == GLFW_KEY_P && action == GLFW_RELEASE && game_state != (int)GAME_STATES::MINIGAME_1) {
			if (registry.walls.size() == 0) {
				printf("\n");
				for (int i = 0; i < 36; ++i) {
					printf("{");
					for (int j = 0; j < 64; ++j) {
						switch (game_state) {
						case (int)GAME_STATES::ORGAN_1:
							printf("%d", ORGAN_1_BOUNDARY[i][j]);
							if (ORGAN_1_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::ORGAN_2:
							printf("%d", ORGAN_2_BOUNDARY[i][j]);
							if (ORGAN_2_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::ORGAN_3:
							printf("%d", ORGAN_3_BOUNDARY[i][j]);
							if (ORGAN_3_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::ORGAN_4:
							printf("%d", ORGAN_4_BOUNDARY[i][j]);
							if (ORGAN_4_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::ORGAN_5:
							printf("%d", ORGAN_5_BOUNDARY[i][j]);
							if (ORGAN_5_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::BRAIN_LOCKED:
							printf("%d", BRAIN_LOCKED_BOUNDARY[i][j]);
							if (BRAIN_LOCKED_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						case (int)GAME_STATES::BRAIN_UNLOCKED:
							printf("%d", BRAIN_UNLOCKED_BOUNDARY[i][j]);
							if (BRAIN_UNLOCKED_BOUNDARY[i][j]) createWall(renderer, vec2((25.0f / 2.0f) + 25 * j, (25.0f / 2.0f) + 25 * i), { 25,-25 });
							break;
						}

						if (j != 63) printf(", ");
					}	
					printf("},\n");
				}
				printf("Done\n");
			}
			else {
				while (registry.walls.entities.size() > 0) {
					registry.remove_all_components_of(registry.walls.entities.back());
				}
			}
					if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
			player_saved_pos = NODE_POS_5_TO_4;
			change_game_states(GAME_STATES::ORGAN_5);
		}
		}
	}

	// SAVE KEY
	if (key == GLFW_KEY_M && action == GLFW_RELEASE && (
		game_state == (unsigned int)GAME_STATES::ORGAN_1 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_2 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_3 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_4 ||
		game_state == (unsigned int)GAME_STATES::ORGAN_5 ||
		game_state == (unsigned int)GAME_STATES::BRAIN_LOCKED ||
		game_state == (unsigned int)GAME_STATES::BRAIN_UNLOCKED
		)) {
		save_game_saveFile("SAVED GAME");
		Mix_PlayChannel(-1, save_game_sound, 0);
		printf("Saved JSON file");
	}

}


/* Debug stuff to draw the boundaries. Commented out for now*/
void WorldSystem::on_mouse_click(int button, int action, int mod) {
	if (debugging.in_dev_mode) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			double xpos, ypos;
			//getting cursor position
			glfwGetCursorPos(window, &xpos, &ypos);
			int xVal = (int)xpos / 25;
			int yVal = (int)ypos / 25;
			float ogX = (25.0f / 2.0f) + 25.0f * xVal;
			float ogY = (25.0f / 2.0f) + 25.0f * yVal;
			debugArray[yVal][xVal] = 1;
			createWall(renderer, vec2(ogX, ogY), { 25,-25 });
		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// xpos and ypos are relative to the top-left of the window
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}