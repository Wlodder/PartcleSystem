#include "ParticleEmitter.hpp"

vec3 ParticleEmitter::sampleVelocity()
{
	return { velocity[0] + Random::Float() * velocity_std[0],
			velocity[1] + Random::Float() * velocity_std[1],
			velocity[2] + Random::Float() * velocity_std[2] };
};
