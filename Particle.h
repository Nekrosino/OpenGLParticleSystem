#pragma once
#include <glm/glm.hpp>

struct Particle
{
	glm::vec2 position;  // Position of the particle
	glm::vec2 velocity;  // Velocity of the particle
	float lifetime = 0.0f; // Remaining lifetime of the particle
	glm::vec4 color = glm::vec4(1.0f); // Color of the particle

	Particle() = default;
	
	Particle(glm::vec2 pos, glm::vec2 vel, float lifetime, glm::vec4 color)
		: position(pos), velocity(vel), lifetime(lifetime), color(color) {
	}


};