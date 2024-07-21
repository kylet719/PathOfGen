#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include "mg4_physics.hpp"

// the background
Entity createBackground(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos, vec2 scale);
// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// the player
Entity createEvilVirus(RenderSystem* renderer, vec2 pos);
// rbc enemy
Entity createRedBloodCell(vec2 pos);
// minigame nodes
Entity createMinigameNode(RenderSystem* renderer, vec2 pos, enum GAME_STATES minigame);
// transport nodes
Entity createTransportNode(vec2 pos, enum GAME_STATES nextOrgan);
// brain item check node
Entity createBrainItemCheckNode(vec2 pos);
// brain ending choice nodes
Entity createBrainEndingChoiceNode(vec2 pos);
// Dialogue or text boxes
Entity createGenericSpaceBarTextBox(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos);
// ATP items
Entity createItem_ATP(RenderSystem* renderer, vec2 pos);
// fat globules
Entity createItem_Lipid(RenderSystem* renderer, vec2 pos, float x_scale_multiplier);
// Arrows that point to the nodes
Entity createArrow(RenderSystem* renderer, vec2 pos, Entity& node);
// mesh object
Entity createMG2MeshObject(RenderSystem* renderer, vec2 pos, vec2 scale, int index, float random);
// mg2 progress bar
Entity createMG2ProgressBar(vec2 pos, vec2 scale);
// mg2 progress
Entity createMG2Progress(vec2 pos, vec2 scale);
// Tutorial
Entity createTutorial(vec2 pos, TEXTURE_ASSET_ID assetID);
// Walls
Entity createWall(RenderSystem* renderer, vec2 pos, vec2 scale);
// Animations
Entity createAnimation(vec2 pos, TEXTURE_ASSET_ID sprite_sheet, int total_frames, vec2 scale);
// Text
Entity createText(const std::string& str, const vec2& pos, float scale, const vec3& color);
// Specific create text for fps
Entity createFpsText();
// Evil virus for whack a mole
Entity createWhackAMole(RenderSystem* renderer, vec2 pos);
// To cover up the viruses
Entity createMg4BackgroundOverlay(RenderSystem* renderer, enum TEXTURE_ASSET_ID assetID, vec2 pos, vec2 scale);
// iron consumables for mg4
void createIron();

Entity createAcid(vec2 pos);

Entity createPlatform(vec2 pos, vec2 scale, float y_velocity);

InstanceRenderRequest createInstanceRender(RenderSystem* renderer, TEXTURE_ASSET_ID assetID, EFFECT_ASSET_ID effectID, GEOMETRY_BUFFER_ID geometryID, INSTANCING_BUFFER_ID instancingID, Entity entity, std::vector<vec2> translations);
// Create mg5 ball
Entity createBall(vec2 pos, vec2 velocity);
// Create mg5 paddle
Entity createPaddle();
// create mg5 brick
Entity createBrick(vec2 pos, vec2 scale, bool hasOxygen, PowerUpType powerUp);
// create mg5 oxygen
Entity createOxygen(vec2 pos);
// create mg5 powerUp
Entity createPowerUp(vec2 pos, PowerUpType powerUp);
// Generic textures for visuals
Entity createGenericTexture(vec2 pos, vec2 scale, enum TEXTURE_ASSET_ID texture);
// create final animation cut scene
Entity createFinalCutSceneAnimation(TEXTURE_ASSET_ID sprite_sheet, int total_x_frames, int total_y_frames, int total_frames);
// Particles
Entity createParticle(vec2 pos, vec3 color);
// create glucose for mg3
Entity createGlucose(vec2 pos, float y_velocity);
// create Finish Line
Entity createFinishLine(vec2 pos, float y_velocity);

