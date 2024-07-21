#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{

};

// Harms the player
struct Deadly
{
	std::vector<Direction> chase;
};

// Player collects
struct Consumable
{

};

// All data relevant to fixed (position) entities 
struct backgroundMotions {
	vec2 position = { 0, 0 };
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	float angle = 0;
};

// Nodes have an associated minigame
struct GameNode {
	unsigned int minigame;
};

struct TransportNode {
	unsigned int nextOrgan;
};

// All data relevant to the shape and motion of entities
struct foregroundMotion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 accel = { 0, 0};
};

// All data relevant to fixed (position) entities 
struct overlayMotions {
	vec2 position = { 0, 0 };
	vec2 scale = { 10, 10 };
	float angle = 0;
};

// entities that can collide
struct Collidable {
	/*float left;
	float right;
	float top;
	float bottom;*/
	//bool isColliding;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
	bool in_dev_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
	float fadeOutTimer = 1500.f;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};


// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
	unsigned int index;
};

struct Arrow {
	Entity associatedNode;
};

struct Background {

};

struct Wall {
	vec2 pos{ 0,0 };
};

struct Text {
	std::string str;
	vec2 pos;
	float scale;
	vec3 color;
	mat4 trans;
};

// MG3 Whack a mole
struct WhackAMole {
	bool active = 0;
	bool whacked = 0;
	float angerLevel = 0;
	vec2 origin = { 0, 0 };
	bool exploded = 0;
};

struct Title {
	unsigned int selectionOption = 0;
	bool titleInPlace = false;
};

struct Credits {
	bool creditsStarted = false;
	bool creditsFinished = false;
};

struct SavedGameTimer {
	float counter_ms = 1500;
};

struct Bar
{

};

struct Random {
	float random;
};
struct Platform {
	bool below = true;
};

struct Jump {
	
};

struct Ball {

};

struct Brick {
	bool hasOxygen = false;
	unsigned int powerUpType;
};

struct BezierCurve {
	float t;
	/*
	* Ordering of the points matters.
	* The first point in the list is the starting point (t = 0.0).
	* The last point in the list is the end point (t = 1.0).
	* All remaining points are control points in order such that the next point gets closer to the end point.
	*/
	std::vector<vec2> points; 
};

struct BrainItemCheckNode {

};

struct BrainEndingChoiceNode {
	bool killEnding; // 1 is kill ending, 0 is help ending
};

struct PowerUp {
	unsigned int type;
};

struct Paddle {
	float longPaddle_ms = 0.f;
};

struct FinishLine {
	
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

// Enumerate the extures
enum class TEXTURE_ASSET_ID {
	// overworld
	GEN = 0,
	TITLE_BACKGROUND = GEN + 1,
	TITLE_ARROW = TITLE_BACKGROUND + 1,
	TITLE_OPTIONS = TITLE_ARROW + 1,
	TITLE_SPLASH = TITLE_OPTIONS + 1,
	TITLE_CONTROLS = TITLE_SPLASH + 1,
	OVERALL_TUTORIAL = TITLE_CONTROLS + 1,
	GAME_NODE = OVERALL_TUTORIAL + 1,
	TRANSPORT_NODE = GAME_NODE + 1,
	TEXT_BOX = TRANSPORT_NODE + 1,
	ARROW = TEXT_BOX + 1,
	MINIGAME_OVER = ARROW + 1,
	MINIGAME_WIN = MINIGAME_OVER + 1,
	// Organ 1
	ORGAN_1_BACKGROUND = MINIGAME_WIN + 1,
	// mini game 1
	MINIGAME_1 = ORGAN_1_BACKGROUND + 1,
	MG1_TUTORIAL = MINIGAME_1 + 1,
	ITEM_ATP = MG1_TUTORIAL + 1,
	EVIL_VIRUS = ITEM_ATP + 1,
	WALL = EVIL_VIRUS + 1,
	RBC_SHEET = WALL + 1,
	MG1_WIN_SHEET = RBC_SHEET + 1,
	MG1_LOSE_SHEET = MG1_WIN_SHEET + 1, 

	// Organ 2
	ORGAN_2_BACKGROUND = MG1_LOSE_SHEET + 1,
	// mini game 2
	MINIGAME_2 = ORGAN_2_BACKGROUND + 1,
	ITEM_LIPID = MINIGAME_2 + 1,
	PROGRESS_BAR = ITEM_LIPID + 1,
	GREEN_BOX = PROGRESS_BAR + 1,
	MG2_TUTORIAL = GREEN_BOX + 1,
	MG2_WIN_SHEET = MG2_TUTORIAL + 1,
	MG2_LOSE_SHEET = MG2_WIN_SHEET + 1,

	// minigame 3
	ORGAN_3_BACKGROUND = MG2_LOSE_SHEET + 1,
	MG3_BACKGROUND = ORGAN_3_BACKGROUND + 1,
	MG3_TUTORIAL = MG3_BACKGROUND + 1,
	MG3_ACID_SHEET = MG3_TUTORIAL + 1,
	MG3_PLATFORM = MG3_ACID_SHEET + 1,
	MG3_GLUCOSE = MG3_PLATFORM + 1,
	MG3_FINISH_LINE = MG3_GLUCOSE + 1,
	MG3_WIN_SHEET = MG3_FINISH_LINE + 1, 
	MG3_LOSE_SHEET = MG3_WIN_SHEET + 1,

	// organ 4
	ORGAN_4_BACKGROUND = MG3_LOSE_SHEET + 1,
	// mini game 4
	MINIGAME_4 = ORGAN_4_BACKGROUND + 1,
	MINIGAME_4_OVERLAY = MINIGAME_4 + 1,
	MG4_TUTORIAL = MINIGAME_4_OVERLAY + 1,
	MG4_EXPLOSION = MG4_TUTORIAL +1,
	IRON = MG4_EXPLOSION + 1,
	MG4_WIN_SHEET = IRON + 1,
	MG4_LOSE_SHEET = MG4_WIN_SHEET + 1, 

	// organ 5
	ORGAN_5_BACKGROUND = MG4_LOSE_SHEET + 1,
	// mg5
	MG5_BACKGROUND = ORGAN_5_BACKGROUND + 1,
	MG5_TUTORIAL = MG5_BACKGROUND + 1,
	PADDLE = MG5_TUTORIAL + 1,
	OXYGEN = PADDLE + 1,
	BRICK = OXYGEN + 1,
	MULTIPLY_3X = BRICK + 1,
	LONGPADDLE = MULTIPLY_3X + 1,
	MG5_WIN_SHEET = LONGPADDLE + 1,
	MG5_LOSE_SHEET = MG5_WIN_SHEET + 1, 

	// brain
	BRAIN_LOCKED_BACKGROUND = MG5_LOSE_SHEET + 1,
	BRAIN_UNLOCKED_BACKGROUND = BRAIN_LOCKED_BACKGROUND + 1,
	BRAIN_KILL_ANIMATION = BRAIN_UNLOCKED_BACKGROUND + 1,
	BRAIN_HELP_ANIMATION = BRAIN_KILL_ANIMATION + 1,

	// credits
	CREDITS_BG = BRAIN_HELP_ANIMATION + 1,
	CREDITS_PILL = CREDITS_BG + 1,
	CREDITS_POISON = CREDITS_PILL + 1,
	CREDITS_ZOMBIE_MAN = CREDITS_POISON +1,
	CREDITS_MAN = CREDITS_ZOMBIE_MAN + 1,

	// general
	BLANK_OVERLAY = CREDITS_MAN + 1,
	PARTICLE = BLANK_OVERLAY + 1,
	

	// Just adding this here for now, gonna take it out once the MG3 item is implemented
	ITEM_GLUCOSE = PARTICLE + 1,

	
	// end
	TEXTURE_COUNT = ITEM_GLUCOSE + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

// Enumerate the effects
enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TEXTURED = COLOURED + 1,
	TRANSITION = TEXTURED + 1,
	MESH = TRANSITION + 1,
	FONT = MESH + 1,
	MOLE = FONT + 1,
	ANIMATION = MOLE + 1,
	EFFECT_COUNT = ANIMATION + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

// Enumerate geometry
enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	SCREEN_TRIANGLE = SPRITE + 1,
	BACKGROUND = SCREEN_TRIANGLE + 1,
	MESH1 = BACKGROUND + 1,
	MESH2 = MESH1 + 1,
	MESH3 = MESH2 + 1,
	GEOMETRY_COUNT = MESH3 + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

// Organs and minigames MUST be together and in ascending order
enum class GAME_STATES {
	TITLE = 0,
	ORGAN_1 = TITLE + 1,
	MINIGAME_1 = ORGAN_1 + 1,
	ORGAN_2 = MINIGAME_1 + 1,
	MINIGAME_2 = ORGAN_2 + 1,
	ORGAN_3 = MINIGAME_2 + 1,
	MINIGAME_3 = ORGAN_3 + 1,
	ORGAN_4 = MINIGAME_3 + 1,
	MINIGAME_4 = ORGAN_4 + 1,
	ORGAN_5 = MINIGAME_4 + 1,
	MINIGAME_5 = ORGAN_5 + 1,
	BRAIN_LOCKED = MINIGAME_5 + 1,
	BRAIN_UNLOCKED = BRAIN_LOCKED + 1,
	BRAIN_KILL = BRAIN_UNLOCKED + 1,
	BRAIN_HELP = BRAIN_KILL + 1,
	CREDITS = BRAIN_HELP + 1,
	GAME_STATES_COUNT = CREDITS + 1
};
const int game_states_count = (int)GAME_STATES::GAME_STATES_COUNT;


enum class TITLE_OPTIONS_SELECTION {
	NEWGAME = 0,
	LOAD = NEWGAME + 1,
	CREDITS = LOAD + 1,
};

enum class INSTANCING_BUFFER_ID {
	ATP = 0,
	PARTICLE = ATP + 1,
	INSTANCING_COUNT = PARTICLE + 1
};
const int instancing_count = (int)INSTANCING_BUFFER_ID::INSTANCING_COUNT;

// Items are represented with a GEOMETRY (like a clister of vertices)
// They have an associated TEXTURE (like what image to map onto it)
// They also have an EFFECT (for shaders to do fun things)
// Then are put in as a request to be rendered
struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

struct TextRenderRequest {

};

enum class PowerUpType {
	NONE = 0,
	MULTIPLY = NONE + 1,
	LONGPADDLE = MULTIPLY + 1
};

struct InstanceRenderRequest {
	unsigned int instances;
	std::vector<vec2> translations; 
	INSTANCING_BUFFER_ID used_instancing = INSTANCING_BUFFER_ID::INSTANCING_COUNT;
};

struct Particle {
	vec3  color;
	float opacity;
    float life;
};

struct Animation
{
	int total_frames;
	int columns;
	int rows = 1;
	int current_frame = 0;
	int current_x_frame = 0;
	int current_y_frame = 1;
	float elapsed_ms = 100.0f; // 1 second per frame
};

