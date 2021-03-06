#pragma once
#include <glm/glm.hpp>

class Particle {

public:
	glm::vec3 pos, velocity;
	glm::vec4 colour;
	float size;
	float life; // Remaining life of the particle. if <0 : dead and unused.

};