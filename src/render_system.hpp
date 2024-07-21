#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// fonts
// NEED FOR FONTS
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

// NEED FOR FONTS
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MESH1, mesh_path("meshObstacle1.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MESH2, mesh_path("meshObstacle2.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::MESH3, mesh_path("meshObstacle3.obj")),
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
		// title
		textures_path("gen.png"),
		textures_path("title/title_background.png"),
		textures_path("title/title_arrow.png"),
		textures_path("title/title_options.png"),
		textures_path("title/title_splash.png"),
		textures_path("title/title_selection_controls.png"),
		textures_path("overall_tutorial.png"),
		textures_path("map_node.png"),
		textures_path("transport_node.png"),
		textures_path("enter_game_dialogue.png"),
		textures_path("arrow.png"),
		textures_path("game_over_overlay.png"),
		textures_path("minigame_win_overlay.png"),
		// organ 1
		textures_path("organ1/organ1_background.png"),
		// mini game 1
		textures_path("minigame1/minigame_1_background.png"),
		textures_path("minigame1/mg1_tutorial.png"),
		textures_path("minigame1/item_atp.png"),
		textures_path("minigame1/evil_cell.png"),
		textures_path("minigame1/wall.png"),
		textures_path("rbc_animation/rbc_sheet.png"),
		textures_path("minigame1/game_win/mg1_win_sheet.png"),
		textures_path("minigame1/game_over/mg1_lose_sheet.png"),
		// organ 2
		textures_path("organ2/organ2_background.png"),
		// mini game 2
		textures_path("minigame2/minigame_2_background.png"),
		textures_path("minigame2/lipid.png"),
		textures_path("minigame2/empty_progress_bar.png"),
		textures_path("minigame2/green_box.png"),
		textures_path("minigame2/mg2_tutorial2.png"),
		textures_path("minigame2/mg2_win/mg2_win_sheet.png"),
		textures_path("minigame2/mg2_lose/mg2_lose_sheet.png"),
		// mini game 3
		textures_path("organ3/organ3_background.png"),
		textures_path("minigame3/mg_3_background.png"),
		textures_path("minigame3/mg3_tutorial.png"),
		textures_path("minigame3/Acid_long_sheet.png"),
		textures_path("minigame3/platform.png"),
		textures_path("minigame3/glucose2.png"),
		textures_path("minigame3/checkered_flag.png"),
		textures_path("minigame3/mg3_win/mg3_gamewin-Sheet.png"),
		textures_path("minigame3/mg3_lose/mg3_lose_sheet.png"),

		// organ 4
		textures_path("organ4/organ4_background.png"),
		// mini game 4
		textures_path("minigame4/minigame_4_background.png"),
		textures_path("minigame4/minigame_4_background_overlay.png"),
		textures_path("minigame4/mg4_tutorial.png"),
		textures_path("minigame4/mg4_explosion.png"),
		textures_path("minigame4/iron.png"),
		textures_path("minigame4/mg4_win/mg4_win_sheet.png"),
		textures_path("minigame4/mg4_lose/mg4_lose_sheet.png"),
		
		// organ 5
		textures_path("organ5/organ5_background.png"),
		// mg5
		textures_path("minigame5/mg_5_background.png"),
		textures_path("minigame5/mg5_tutorial.png"),
		textures_path("minigame5/paddle.png"),
		textures_path("minigame5/oxygen.png"),
		textures_path("minigame5/brick.png"),
		textures_path("minigame5/multiply_3x.png"),
		textures_path("minigame5/longPaddle.png"),
		textures_path("minigame5/mg5_win/mg5_win_sheet.png"),
		textures_path("minigame5/mg5_lose/mg5_lose_sheet.png"),

		// Brain
		textures_path("brain/brain_locked_background.png"),
		textures_path("brain/brain_unlocked_background.png"),
		textures_path("brain/kill/kill_sheet_162.png"),
		textures_path("brain/help/help_sheet_143.png"),
		
		// credits
		textures_path("credits/credits_bg.png"),
		textures_path("brain/help/pill.png"),
		textures_path("brain/kill/poison.png"),
		textures_path("brain/help/zombie_man_credits.png"),
		textures_path("brain/kill/man_credits.png"),

		// General
		textures_path("blank_overlay.png"),
		textures_path("particle.png"),

		// Just adding this here for now, gonna take it out once the MG3 item is implemented
		textures_path("minigame3/glucose2.png")
	};
			

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("transition"),
		shader_path("mesh"),
		shader_path("font"),
		shader_path("mole"),
		shader_path("animation")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;
	std::array<GLuint, instancing_count> instancing_buffers;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	// font stuff
	bool fontInit(GLFWwindow* window, const std::string& font_filename, unsigned int font_default_size);
	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4 trans);
	void initializeInstanceBuffers();

	Entity screen_state_entity;

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity& entity, const mat3& projection);
	void drawToScreen();
	GLuint getForegroundTexture(Entity& entity, RenderRequest& render_request);
	//GLuint runOverlayAnimation(Entity& entity);

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	// fonts
	// NEED FOR FONTS
	std::map<char, Character> m_ftCharacters;
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;
	// work around for fonts
	GLuint dummy_vao;
	GLint texture_in_offset = 2;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
