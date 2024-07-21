#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <random>

Entity createBackground(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos, vec2 scale)
{
	auto entity = Entity();

	// Initialize the motion
	auto& staticObject = registry.backgroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.scale = scale;

	registry.background.emplace(entity);

	// Calls render request to be rendered later
	registry.backgroundRenderRequests.insert(
		entity,
		{ assetID,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::BACKGROUND});

	return entity;
}


Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	// Setting initial motion values
	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 100.f;
	motion.scale.y *= -1; // point front to the right
	motion.accel = { 0, 0 };

	registry.players.emplace(entity);
	registry.collidables.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GEN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEvilVirus(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	// Setting initial motion values
	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = mesh.original_size * 100.f;
	motion.scale.y *= -1; // point front to the right
	motion.accel = { 0, 0 };

	registry.deadlys.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EVIL_VIRUS,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createRedBloodCell(vec2 pos)
{
	auto entity = Entity();

	//Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale *= -12; // point to the left and upright
	motion.accel = { 0, 0 };

	registry.deadlys.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::RBC_SHEET,
			EFFECT_ASSET_ID::ANIMATION,
			GEOMETRY_BUFFER_ID::SPRITE });

	Animation& animation = registry.animation.emplace(entity);
	animation.elapsed_ms = 75.f;
	animation.columns = 7;
	animation.rows = 1;
	animation.total_frames = 7;

	return entity;
}

Entity createMinigameNode(RenderSystem* renderer, vec2 pos, enum GAME_STATES minigame)
{
	auto entity = Entity();

	backgroundMotions& staticObject = registry.backgroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.angle = 0.f;
	staticObject.scale *= vec2(8, - 7); // point front to the right

	registry.gameNodes.emplace(entity);
	registry.gameNodes.get(entity).minigame = (unsigned int)minigame;
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GAME_NODE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTransportNode(vec2 pos, enum GAME_STATES nextGameState)
{
	auto entity = Entity();

	backgroundMotions& staticObject = registry.backgroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.angle = 0.f;
	staticObject.scale *= vec2{10, -10}; // point front to the right

	registry.transportNodes.emplace(entity);
	registry.transportNodes.get(entity).nextOrgan = (unsigned int)nextGameState;
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TRANSPORT_NODE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBrainItemCheckNode(vec2 pos) {
	auto entity = Entity();

	backgroundMotions& staticObject = registry.backgroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.angle = 0.f;
	staticObject.scale *= vec2{ 10, -10 }; // point front to the right

	registry.brainItemCheckNode.emplace(entity);
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GAME_NODE, // Just for the visuals
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBrainEndingChoiceNode(vec2 pos) {
	auto entity = Entity();

	backgroundMotions& staticObject = registry.backgroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.angle = 0.f;
	staticObject.scale *= vec2{ 10, -10 }; // point front to the right

	registry.brainEndingChoiceNode.emplace(entity);
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GAME_NODE, // Just for the visuals
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createGenericSpaceBarTextBox(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	overlayMotions& overlayMotions = registry.overlayMotions.emplace(entity);
	overlayMotions.position = pos;
	overlayMotions.angle = 0.f;
	
	overlayMotions.scale = { mesh.original_size[0] * 1600.f, mesh.original_size[1] * 900.f };
	overlayMotions.scale.y *= -1; // point front to the right

	return entity;
}

Entity createItem_ATP(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	//motion.velocity = { 100.0f, 0.0f };
	motion.velocity = { -1.0f, 0.0f };
	motion.scale = mesh.original_size * 50.f;
	motion.scale.y *= -1; // point front to the right

	registry.consumables.emplace(entity);
	registry.collidables.emplace(entity);

	return entity;
}

Entity createItem_Lipid(RenderSystem* renderer, vec2 pos, float x_scale_multiplier) {
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale *= 5.f;
	motion.scale.x *= x_scale_multiplier;
	motion.scale.y *= -1;

	registry.consumables.emplace(entity);
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ITEM_LIPID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createArrow(RenderSystem* renderer, vec2 pos, Entity& node) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);

	backgroundMotions& motion = registry.backgroundMotions.emplace(entity);
	motion.position = pos;
	motion.velocity = { 0.0f, -100.0f };
	motion.scale = mesh.original_size * 50.f;
	motion.scale.y *= -1; // point front to the right

	registry.arrows.emplace(entity);
	registry.arrows.get(entity).associatedNode = node;

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createMG2MeshObject(RenderSystem* renderer, vec2 pos, vec2 scale, int index, float random)
{
	auto entity = Entity();
	int mesh_index = (int)GEOMETRY_BUFFER_ID::MESH1 + index;

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh((GEOMETRY_BUFFER_ID)mesh_index);
	registry.meshPtrs.emplace(entity, &mesh);
	mesh.index = index;

	// gives each mesh some random value to carry forward
	Random& r = registry.random.emplace(entity);
	r.random = random;

	// Setting initial motion values
	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale.x = mesh.original_size.x * scale.x;
	motion.scale.y = mesh.original_size.y * scale.y;
	motion.scale.y *= -1; // point front to the right

	registry.deadlys.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::MESH,
			(GEOMETRY_BUFFER_ID)mesh_index });

	return entity;
}

Entity createMG2ProgressBar(vec2 pos, vec2 scale) {
	auto entity = Entity();

	overlayMotions& motion = registry.overlayMotions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	registry.overlayRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PROGRESS_BAR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createMG2Progress(vec2 pos, vec2 scale) {
	auto entity = Entity();

	overlayMotions& motion = registry.overlayMotions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	registry.bar.emplace(entity);

	registry.overlayRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GREEN_BOX,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createTutorial(vec2 pos, TEXTURE_ASSET_ID assetId) {
	auto entity = Entity();

	overlayMotions& overlayMotions = registry.overlayMotions.emplace(entity);
	overlayMotions.position = pos;
	overlayMotions.angle = 0.f;
	overlayMotions.scale = { 1000, 400 };
	overlayMotions.scale.y *= -1; // point front to the right

	registry.overlayRenderRequests.insert(
		entity,
		{
			assetId,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE	
		}

	);

	return entity;

}

Entity createWall(RenderSystem* renderer, vec2 pos, vec2 scale) {
	auto entity = Entity();

	backgroundMotions& motion = registry.backgroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	registry.walls.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WALL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createPlatform(vec2 pos, vec2 scale, float y_velocity)
{
	auto entity_one = Entity();

	foregroundMotion& motion_one = registry.foregroundMotions.emplace(entity_one);
	motion_one.position = pos;
	motion_one.scale = scale;
	motion_one.velocity.y = y_velocity;

	registry.platform.emplace(entity_one);
	registry.collidables.emplace(entity_one);

	registry.backgroundRenderRequests.insert(
		entity_one,
		{ TEXTURE_ASSET_ID::MG3_PLATFORM,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity_one;
}

Entity createAnimation(vec2 pos, TEXTURE_ASSET_ID sprite_sheet, int total_frames, vec2 scale)
{
	auto entity = Entity();

	overlayMotions& overlayMotions = registry.overlayMotions.emplace(entity);
	overlayMotions.position = pos;
	overlayMotions.angle = 0.f;
	overlayMotions.scale = scale;
	overlayMotions.scale.y *= -1; // point front to the right

	registry.overlayRenderRequests.insert(
		entity,
		{
			sprite_sheet,
			EFFECT_ASSET_ID::ANIMATION,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	Animation& animation = registry.animation.emplace(entity);
	animation.total_frames = total_frames;
	animation.columns = total_frames;
	animation.rows = 1;
	animation.current_x_frame = 0;

	return entity;
}

Entity createFinalCutSceneAnimation(TEXTURE_ASSET_ID sprite_sheet, int total_x_frames, int total_y_frames, int total_frames)
{
	auto entity = Entity();

	overlayMotions& overlayMotions = registry.overlayMotions.emplace(entity);
	overlayMotions.position = vec2(window_width_px / 2, window_height_px / 2);
	overlayMotions.angle = 0.f;
	overlayMotions.scale = { 1600, 900 };
	overlayMotions.scale.y *= -1; // point front to the right

	registry.overlayRenderRequests.insert(
		entity,
		{
			sprite_sheet,
			EFFECT_ASSET_ID::ANIMATION,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	Animation& animation = registry.animation.emplace(entity);
	animation.total_frames = total_frames;
	animation.columns = total_x_frames;
	animation.rows = total_y_frames;

	return entity;
}

Entity createText(const std::string& str, const glm::vec2& pos, float scale, const glm::vec3& color) {
	Entity entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.str = str;
	text.pos = pos;
	text.scale = scale;
	text.color = color;
	text.trans = mat4(1.0f);

	registry.textRenderRequests.emplace(entity);

	return entity;
}

Entity createFpsText() {
	Entity entity = Entity();

	Text& text = registry.texts.emplace(entity);
	text.str = "";
	text.pos = vec2(0.0f, 0.0f);
	text.scale = 1.0f;
	text.color = vec3(1.0f, 1.0f, 1.0f);
	text.trans = mat4(1.0f);

	registry.textRenderRequests.emplace(entity);

	return entity;
}

// Minigame 3

Entity createWhackAMole(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Setting initial motion values
	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = { 100, -100 }; // point front to the right
	motion.accel = { 0, 0 };

	registry.whackAMole.emplace(entity);
	registry.whackAMole.get(entity).origin = pos;
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EVIL_VIRUS,
			EFFECT_ASSET_ID::MOLE,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createMg4BackgroundOverlay(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos, vec2 scale)
{
	auto entity = Entity();

	// Initialize the motion
	auto& staticObject = registry.foregroundMotions.emplace(entity);
	staticObject.position = pos;
	staticObject.scale = scale;

	// Calls render request to be rendered later
	registry.foregroundRenderRequests.insert(
		entity,
		{ assetID,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::BACKGROUND });

	return entity;
}

void createIron() {
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.scale = vec2(50.f, -50.f);
	
	BezierCurve& bezier = registry.beziers.insert(
		entity,
		{ 0.0f, {} }
	);

	randomizeBezierPoints_MG4(bezier, motion);

	registry.consumables.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::IRON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE 
		}
	);
}

InstanceRenderRequest createInstanceRender(RenderSystem* renderer, TEXTURE_ASSET_ID assetID, EFFECT_ASSET_ID effectID, GEOMETRY_BUFFER_ID geometryID, INSTANCING_BUFFER_ID instancingID, Entity entity, std::vector<vec2> translations)
{
	registry.backgroundRenderRequests.insert(
		entity,
		{ assetID, effectID, geometryID }
	);

	InstanceRenderRequest& instance = registry.instanceRenderRequests.emplace(entity);
	instance.translations = translations;
	instance.instances = translations.size();
	instance.used_instancing = instancingID;

	return instance;
}

Entity createAcid(vec2 pos)
{
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale.x *= -180;
	motion.scale.y *= -10;
	motion.accel = { 0, 0 };

	registry.deadlys.emplace(entity);
	registry.collidables.emplace(entity);
	registry.foregroundRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MG3_ACID_SHEET,
			EFFECT_ASSET_ID::ANIMATION,
			GEOMETRY_BUFFER_ID::SPRITE });

	Animation& animation = registry.animation.emplace(entity);
	animation.elapsed_ms = 75.0f;
	animation.total_frames = 4;
	animation.columns = 4;
	animation.rows = 1;

	return entity;
}

Entity createBall(vec2 pos, vec2 velocity) {
	Entity entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.accel = vec2(0.0f, 0.0f);
	motion.angle = 0.0f;
	motion.position = pos;
	motion.scale = vec2(50.0f, -50.0f);
	motion.velocity = velocity;

	registry.balls.emplace(entity);

	registry.foregroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::GEN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

Entity createPaddle() {
	Entity entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.accel = vec2(0.0f, 0.0f);
	motion.angle = 0.0f;
	motion.position = vec2(window_width_px / 2, window_height_px - 50.0f);
	motion.scale = vec2(150.0f, -25.0f);
	motion.velocity = vec2(0.0f, 0.0f);

	registry.paddles.emplace(entity); 

	registry.foregroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::PADDLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

Entity createBrick(vec2 pos, vec2 scale, bool hasOxygen, PowerUpType powerUp) {
	auto entity = Entity();

	backgroundMotions& motion = registry.backgroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2(scale.x, -scale.y);

	registry.bricks.insert(entity, { hasOxygen , (unsigned int)powerUp });

	registry.backgroundRenderRequests.insert(
		entity,
		{ 
			TEXTURE_ASSET_ID::BRICK,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE 
		}
	);

	return entity;
}

Entity createOxygen(vec2 pos) {
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2(50.f, -50.f);

	registry.consumables.emplace(entity);

	vec2 p1 = vec2(rand() % window_width_px, rand() % (window_height_px / 3) + window_height_px / 2);
	vec2 p2 = vec2(pos.x, window_height_px + abs(motion.scale.y));

	std::vector<vec2> points = { pos, p1, p2 };
	
	registry.beziers.insert(
		entity,
		{ 0.0f, points }
	);

	registry.foregroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::OXYGEN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE 
		}
	);

	return entity;
}

Entity createPowerUp(vec2 pos, PowerUpType powerUp) {
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.velocity = vec2(0.0f, 100.f);
	motion.scale = vec2(50.f, -50.f);

	registry.consumables.emplace(entity);
	registry.powerUps.insert(entity, { (unsigned int)powerUp });

	TEXTURE_ASSET_ID texture;
	if (powerUp == PowerUpType::MULTIPLY) {
		texture = TEXTURE_ASSET_ID::MULTIPLY_3X;
	} else if (powerUp == PowerUpType::LONGPADDLE) {
		texture = TEXTURE_ASSET_ID::LONGPADDLE;
	}

	registry.foregroundRenderRequests.insert(
		entity,
		{
			texture,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE 
		}
	);

	return entity;
}

Entity createGlucose(vec2 pos, float y_velocity)
{
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2(50.f, -50.f);
	motion.angle = 0;
	motion.velocity.y = y_velocity;

	registry.consumables.emplace(entity);
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::MG3_GLUCOSE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

Entity createFinishLine(vec2 pos, float y_velocity)
{
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2(window_width_px, -75.f);
	motion.angle = 0;
	motion.velocity.y = y_velocity;

	registry.finishLine.emplace(entity);
	registry.collidables.emplace(entity);

	registry.backgroundRenderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::MG3_FINISH_LINE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

// Created for animations on the credit screen that only just need movements
Entity createGenericTexture(vec2 pos, vec2 scale, enum TEXTURE_ASSET_ID texture) {
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;


	registry.foregroundRenderRequests.insert(
		entity,
		{
			texture,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}


Entity createParticle(vec2 pos, vec3 color)
{
	auto entity = Entity();

	foregroundMotion& motion = registry.foregroundMotions.emplace(entity);
	motion.position = pos;
	motion.velocity = { 0.0f, 100.0f }; // particle velocity dependent on minigame situation
	motion.scale = { 5.0f, 5.0f };

	Particle& particle = registry.particles.emplace(entity);
	particle.color = color;
	registry.colors.emplace(entity, color);
	particle.opacity = 1.0f;
	particle.life = 0.5f;

	return entity;
}



