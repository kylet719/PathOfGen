// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"


GLuint RenderSystem::getForegroundTexture(Entity& entity, RenderRequest& render_request)
{
	if (render_request.used_effect != EFFECT_ASSET_ID::MESH)
	{
		return texture_gl_handles[(GLuint)registry.foregroundRenderRequests.get(entity).used_texture];
	}
	else
	{
		return (GLuint)TEXTURE_ASSET_ID::TEXTURE_COUNT;
	}
}

void RenderSystem::drawTexturedMesh(Entity& entity,
									const mat3 &projection)
{
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	RenderRequest render_request;
	GLuint texture_id;

	// Transformations
	if (registry.foregroundMotions.has(entity)) {
		foregroundMotion& motion = registry.foregroundMotions.get(entity);
		transform.translate(motion.position);
		transform.scale(motion.scale);
		transform.rotate(motion.angle);
	}
	else if (registry.backgroundMotions.has(entity)) {
		backgroundMotions& backgroundMotions = registry.backgroundMotions.get(entity);
		transform.translate(backgroundMotions.position);
		transform.scale(backgroundMotions.scale);
		transform.rotate(backgroundMotions.angle);
	}
	else if (registry.overlayMotions.has(entity)) {
		overlayMotions& overlayMotions = registry.overlayMotions.get(entity);
		transform.translate(overlayMotions.position);
		transform.scale(overlayMotions.scale);
		transform.rotate(overlayMotions.angle);
	}

	// Rendering order
	if (registry.backgroundRenderRequests.has(entity)) {
		render_request = registry.backgroundRenderRequests.get(entity);
		texture_id =
			texture_gl_handles[(GLuint)registry.backgroundRenderRequests.get(entity).used_texture];
	}
	else if (registry.foregroundRenderRequests.has(entity)) {
		render_request = registry.foregroundRenderRequests.get(entity);
		texture_id = getForegroundTexture(entity, render_request);
	}
	else if (registry.overlayRenderRequests.has(entity)) {
		render_request = registry.overlayRenderRequests.get(entity);
		texture_id = texture_gl_handles[(GLuint)registry.overlayRenderRequests.get(entity).used_texture];
	}
	else {
		assert(false);
	}

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		GLint is_instanced_loc = glGetUniformLocation(program, "is_instanced");

		gl_has_errors();
		assert(in_texcoord_loc >= 0);
		assert(in_position_loc >= 0);
		assert(is_instanced_loc >= 0);

		if (registry.instanceRenderRequests.has(entity))
		{
			// set instance rendering to true in vertex shader
			glUniform1i(is_instanced_loc, 1);

			InstanceRenderRequest& irr = registry.instanceRenderRequests.get(entity);

			GLuint instance_vbo = instancing_buffers[(int)irr.used_instancing];
			glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * irr.instances, &irr.translations[0], GL_STATIC_DRAW);
			gl_has_errors();

			glEnableVertexAttribArray(texture_in_offset);
			glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
			glVertexAttribPointer(texture_in_offset, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glVertexAttribDivisor(texture_in_offset, 1);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			gl_has_errors();
		}
		else
		{
			// set instance rendering to false in vertex shader
			glUniform1i(is_instanced_loc, 0);
			gl_has_errors();
		}

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		gl_has_errors();

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
		
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::MESH)
	{
		// Code left in for reference to specific effects on things

		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	} else if (render_request.used_effect == EFFECT_ASSET_ID::MOLE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		GLint whacked_uloc = glGetUniformLocation(program, "whacked");
		GLfloat anger_level_uloc = glGetUniformLocation(program, "anger_level");
		GLuint time_uloc = glGetUniformLocation(program, "time");

		assert(whacked_uloc >= 0);
		assert(in_texcoord_loc >= 0);
		assert(anger_level_uloc >= 0);
		assert(time_uloc >= 0);
		gl_has_errors();
		
		glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
		gl_has_errors();


		if (registry.whackAMole.has(entity)) {
			glUniform1i(whacked_uloc, registry.whackAMole.get(entity).whacked);
			if (registry.whackAMole.get(entity).angerLevel < 0.5) { // If anger level passes this value, start shaking
				glUniform1f(anger_level_uloc, 0);
			}
			else {
				glUniform1f(anger_level_uloc, registry.whackAMole.get(entity).angerLevel-0.5);
			}
		}

		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATION)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		GLint frame_index_loc = glGetUniformLocation(program, "frame_index");
		GLint sheet_width_loc = glGetUniformLocation(program, "sheet_width");
		GLint frame_y_loc = glGetUniformLocation(program, "frame_y");
		GLint sheet_height_loc = glGetUniformLocation(program, "sheet_height");

		gl_has_errors();
		assert(in_texcoord_loc >= 0);
		assert(in_position_loc >= 0);
		assert(frame_index_loc >= 0);
		assert(sheet_width_loc >= 0);
		assert(frame_y_loc >= 0);
		assert(sheet_height_loc >= 0);
		

		Animation& animation = registry.animation.get(entity);
		glUniform1i(frame_index_loc, animation.current_x_frame);
		glUniform1i(sheet_width_loc, animation.columns);
		glUniform1i(frame_y_loc, animation.current_y_frame);
		glUniform1i(sheet_height_loc, animation.rows);
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		gl_has_errors();

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}


	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	GLint opacity_uloc = glGetUniformLocation(program, "opacity");
	const float opacity = registry.particles.has(entity) ? registry.particles.get(entity).opacity : 1.0f;
	glUniform1fv(opacity_uloc, 1, (float*)&opacity);

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();

	// instance rendering is only enabled for textures for now
	if (registry.instanceRenderRequests.has(entity) && render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		InstanceRenderRequest& irr = registry.instanceRenderRequests.get(entity);
		glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr, irr.instances);
	}
	else
	{
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	}
	
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::TRANSITION]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	const GLuint transition_program = effects[(GLuint)EFFECT_ASSET_ID::TRANSITION];
	// Set clock
	GLuint darken_timer_uloc = glGetUniformLocation(transition_program, "darken_screen_factor");

	float& darken_screen_factor = registry.screenStates.get(screen_state_entity).darken_screen_factor;

	glUniform1f(darken_timer_uloc, darken_screen_factor);
	gl_has_errors();
	
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(transition_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(1.0, 0.882, 0.871, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();

	// Draw background images first
	for (Entity entity : registry.backgroundRenderRequests.entities) {
		drawTexturedMesh(entity, projection_2D);
	}

	// Draw foreground objects after
	for (Entity entity : registry.foregroundRenderRequests.entities)
	{
		drawTexturedMesh(entity, projection_2D);
	}

	// Draw overlay objects after
	for (Entity entity : registry.overlayRenderRequests.entities)
	{
		drawTexturedMesh(entity, projection_2D);
	}

	// Draw text objects after
	for (Entity entity: registry.textRenderRequests.entities) {
		Text& text = registry.texts.get(entity);
		if (text.str != "") {
			renderText(text.str, text.pos.x, text.pos.y, text.scale, text.color, text.trans);
		}
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}