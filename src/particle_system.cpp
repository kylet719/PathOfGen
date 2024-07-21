#include "particle_system.hpp"
#include "world_init.hpp"
#include <cmath>

const vec2 gravity = { 0.0f, -9.81f };
const float dragCoefficient = 5.0f;

// Helper: generates random float in the range [min, max]
float randomFloat(float min, float max) {
    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

// Helper: generates a random position within a circle around a position
vec2 randomize_init_position(vec2 position, float radius) {
	float angle = randomFloat(0.0f, 2.0f * M_PI); // Random angle between 0 and 2π
    float r = sqrt(randomFloat(0.0f, 1.0f)) * radius; // Random radius, sqrt for uniform distribution

    // Convert polar to Cartesian coordinates
    float x = r * cos(angle);
    float y = r * sin(angle);

	// Return offset by original position
    return vec2(position.x + x, position.y + y);
}

// Updates all particles currently in the particles registry
void stepParticles(float elapsed_ms) {
    float step_time = elapsed_ms / 1000.f;
	ComponentContainer<Particle>& particlesRegistry = registry.particles;
	for (uint i = 0; i < particlesRegistry.components.size(); i++) {
		Entity p_entity = particlesRegistry.entities[i];
		foregroundMotion& motion = registry.foregroundMotions.get(p_entity);
		
        // move particles
		// calculate velocity after gravity and drag applied
		motion.velocity += gravity * step_time;
		vec2 dragForce = -dragCoefficient * motion.velocity;
		motion.velocity.x += dragForce.x * step_time;
		
		motion.position += motion.velocity * step_time;

        // update translation offsets for instanced particles
		for (Entity particle : registry.particles.entities)
		{
		    InstanceRenderRequest& irr = registry.instanceRenderRequests.get(particle);
		    for (uint i = 1; i < irr.instances; i++) {
                float angle = step_time * 2.0f * M_PI; 
                float deltaX = 0.0005f * std::cos(angle);
                if (randomFloat(0.0f, 10.0f) > 5) { 
                    irr.translations[i][0] += deltaX;
                } else {
                    irr.translations[i][0] -= deltaX;
                }
				irr.translations[i][1] += motion.velocity[0] * step_time / 10000.f;
			}
		}

		Particle& curr_particle = registry.particles.get(p_entity);
        curr_particle.life -= step_time;    // reduce particle life
        if (curr_particle.life > 0.0f)      // particle is alive, thus update
        {	
            curr_particle.opacity -= step_time;
        } 
		else // kill particle
		{
			registry.remove_all_components_of(p_entity);
		}
	} 
}

// returns translations vector to pass into instance rendering
std::vector<vec2> generateParticles(vec2 emitter_position, vec3 color)
{
	std::vector<vec2> translations;
	unsigned int max_particles_per_cluster = 10; 

	// first particle, created in minigame 5 as starting point for instance rendering
	// i.e. initial reference particle i = 0
	translations.push_back({ 0,0 }); // initial offset is 0

	for (unsigned int i = 1; i < max_particles_per_cluster; ++i) {
		// offset calculation 
		// a new particle can be anywhere within a circular area around the emitter position
		vec2 particle_pos = randomize_init_position(emitter_position, 0.08f);
		vec2 offset = particle_pos - emitter_position;
		translations.push_back(offset);
	}

	return translations;
}
