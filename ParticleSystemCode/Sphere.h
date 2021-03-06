#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <cfloat>
#include <cmath>
#include <iostream>
#include "Particle.hpp"
#include "ParticleEmitter.hpp"

class Sphere
{
	// (x - x1)^2 + (y - y1)^2 + (z - z1)^2 = r^2
	glm::vec3 pos;
	float radius;
	float dampening = 0.5;
	const float M_SQRT1_2 = 0.707106781186547524401;
public:
	Sphere(float x, float y, float z, float r) : pos(x, y, z), radius(r) {} ;

	bool operator<=(const Particle& particle) {
		// Sort in reverse order : far particles drawn first.
		return   (pos.x - particle.pos.x) * (pos.x - particle.pos.x) 
			   + (pos.y - particle.pos.y) * (pos.y - particle.pos.y)
			   + (pos.z - particle.pos.z) * (pos.z - particle.pos.z) > radius * radius;
	}

	// bounce particle off plane
	void bounce(Particle& particle, float delta)
	{
		glm::fquat q;
		// axis of rotation = point of collision  = particle position

		glm::vec3 axis = particle.pos;

		// only if pos == origin
		axis *= M_SQRT1_2 / radius;

		q[0] = axis[0];
		q[1] = axis[1];
		q[2] = axis[2];
		q[3] = M_SQRT1_2;

		// reset position to not get stuck in sphere
		particle.pos = particle.pos - particle.velocity * delta;

		// random float for roughness of surface of inside of globe
		
		particle.velocity = (vec3)(abs(Random::Float()), abs(Random::Float()),abs(Random::Float())) + q * particle.velocity * glm::conjugate(q);
		
		// if below the plane of bounce -> inside the sphere -> *-1
		particle.velocity *= -1.0;
		particle.velocity *= dampening;
	}

	void setRadius(float r)
	{
		radius = r;
	}

	void setDampening(float _dampening)
	{
		dampening = _dampening;
	}


};

