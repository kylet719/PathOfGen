#pragma once

#include "game_state.hpp"
#include <random>

void stepParticles(float elapsed_ms);

std::vector<vec2> generateParticles(vec2 emitter_position, vec3 color);