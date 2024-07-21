#pragma once

// internal
#include "common.hpp"
#include "game_state.hpp"

// stlib
#include <vector>
#include <random>
#include <deque>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Enter a minigame
	// Saves the old rendered state to return back
	// Clears the render requests
	void change_game_states(enum GAME_STATES game);

	void create_title();

	// Pacman
	void create_organ_1(vec2 startingPos);
	void create_minigame_1();

	// Falling/moving through blood vessel
	void create_organ_2(vec2 startingPos);
	void create_minigame_2();
	void end_minigame_2(float elapsed_ms_since_last_update);

	// Floor is Lava
	void create_organ_3(vec2 startingPos);
	void create_minigame_3();
	void minigame3_step(float elapsed_ms_since_last_update);

	// Whack a mole
	void create_organ_4(vec2 startingPos);
	void create_minigame_4();
	void minigame4_step(float elapsed_ms_since_last_update);
	void end_minigame_4();

	// Brickbreaker
	void create_organ_5(vec2 startingPos);
	void create_minigame_5();
	void minigame5_step(float elapsed_ms_since_last_update);

	// Brain
	void create_brain(vec2 startingPos);
	void create_brain_kill();
	void create_brain_help();
	bool check_enough_items();

	// credits
	void create_credits();

	// Should the game be over ?
	bool is_over()const;

	// Save/Load game
	void save_game_saveFile(std::string message);
	void load_game_saveFile();

	void display_inventory();

	// screen transitions
	bool fadeOut = false;

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mod);

	void minigame1_maze_gen();
	void minigame1_carve_maze(vec2 pos, std::vector<vec2>& visited);

	// restart level
	void restart_game();

	// create minigame win / lose overlays + animations
	void minigame_win_lose_overlay(bool win, std::vector<Entity>& entities_to_remove, TEXTURE_ASSET_ID used_texture, int num_frames);
	// OpenGL window handle
	GLFWwindow* window;

	// Number of points
	int points;
	// TODO: adjust the required numbers to fit the game
	unsigned int mg1_inv_points = 0;
	const unsigned int MG1_ITEM_GOAL = 100;
	unsigned int mg2_inv_points = 0;
	const unsigned int MG2_ITEM_GOAL = 10;
	unsigned int mg3_inv_points = 0;
	const unsigned int MG3_ITEM_GOAL = 10;
	unsigned int mg4_inv_points = 0;
	const unsigned int MG4_ITEM_GOAL = 10;
	unsigned int mg5_inv_points = 0;
	const unsigned int MG5_ITEM_GOAL = 10;
	bool showGoal = false; // Should we show the number required?

	// used to space out when we update the fps counter text
	int counter = 0;
	Entity fpsTextEntity;

	// Game state
	RenderSystem* renderer;
	Entity player;
	vec2 player_saved_pos; // For when returning back from minigame
	Entity player_mg;
	Entity enemy_rbc;
	int numberOfConsumables = 0;
	enum GAME_STATES currentOrgan;
	unsigned int completedOrgan; // maxCompletedOrgan

	// mg1 variables
	float MAP_BLOCK_SIZE = 100;
	float CURRENT_SPRITE_OFFSET = 50;
	vec2 atpStart = { 150, 50 };
	Entity mg1_score;

	// mg2 variables
	float mg2_duration = 40000.f; // 40 seconds

	// mg3 variables
	float glucose_counter = 2000.f;
	int num_of_glucose = 10;
	float vertical_change = 180;
	std::vector<vec2> platform_pos;

	// mg4 variables
	float ALLOWED_TIME_TO_BE_ALIVE = 1.5;
	unsigned int explosionCount = 0;
	Entity remainingText;

	const int NUM_IRON = 2;

	// mg5 variables
	int numBricks = 0;
	unsigned int BRICK_WIDTH = 100;
	unsigned int BRICK_HEIGHT = 50;
	bool mg5_gameFinished = false;
	Entity remainingBricksText;

	// brain variables
	bool brainGateUnlocked = false;
	Entity brainGateNode;
	bool enoughItems = false;

	// Ending scenes
	bool inEndingScene = false;
	bool endingSceneFinished = false;


	GameState game_state_system;

	bool musicToggle;

	Entity genericSpacebarTextBox;
	bool genericSpacebarTextBoxActive;
	Entity genericTextBoxAccompanyingText;

	std::unordered_map<GAME_STATES, bool> tutorialChecklist{
		{GAME_STATES::TITLE, false},
		{GAME_STATES::MINIGAME_1, false},
		{GAME_STATES::MINIGAME_2, false},
		{GAME_STATES::MINIGAME_3, false},
		{GAME_STATES::MINIGAME_4, false},
		{GAME_STATES::MINIGAME_5, false},
	};

	Entity tutorial;
	vec2 tutorialPos = vec2(window_width_px / 2, window_height_px / 2);

	Entity minigameOver;
	Entity minigameWin;

	// music references
	// Title
	Mix_Music* title_background_music;
	Mix_Chunk* title_select_sound;
	Mix_Chunk* title_enter_sound;

	// Organ/Minigame 1
	Mix_Music* organ1_background_music;
	Mix_Chunk* mg1_die_sound;
	Mix_Chunk* mg1_pickup_atp_sound;
	Mix_Music* mg1_background_music;

	// Organ/Minigame 2
	Mix_Music* organ2_background_music;
	Mix_Music* mg2_background_music;

	// Organ/Minigame 3
	Mix_Music* organ3_background_music;
	Mix_Chunk* mg3_whack_sound;
	Mix_Chunk* mg3_explosion_sound;
	Mix_Music* mg3_background_music;
	
	// Organ/Minigame 4
	Mix_Music* organ4_background_music;
	Mix_Music* mg4_background_music;
	
	// Misc
	Mix_Chunk* overworld_on_node_sound;
	Mix_Chunk* save_game_sound;

	// Organ/Minigame 5
	Mix_Music* organ5_background_music;
	Mix_Music* mg5_background_music;
	Mix_Chunk* mg5_paddle_increase_sound;
	Mix_Chunk* mg5_x3_multiplier_sound;

	// Credits
	Mix_Music* credits_background_music;
	
	// Brain
	Mix_Music* brain_background_music;
	Mix_Music* brain_kill_background_music;
	Mix_Music* brain_help_background_music;
	
	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};

class RollingFps {
public: 
	RollingFps(uint window_size) : window_size(window_size) {}

	void update(float frame_time_ms) {
		frame_times.push_back(frame_time_ms);
		total_time += frame_time_ms;

		if (frame_times.size() > window_size) {
			total_time -= frame_times.front();
			frame_times.pop_front();
		}
	}

	float calculate_fps() const {
		if (frame_times.empty()) return 0.0;
		float average_time_ms = static_cast<float>(total_time) / frame_times.size();
		return 1000.f / average_time_ms;
	}

private:
	std::deque<float> frame_times;
	unsigned int window_size;
	float total_time = 0;
};
