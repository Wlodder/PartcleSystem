#pragma once
#include <glm/glm.hpp>
#include <random>

using namespace glm;


// Contains 
class ParticleEmitter
{
	vec3 position;
	vec3 velocity;

	vec3 velocity_std;
	vec4 colour;
	
	float lifetime;
	float size;

public:

	void setPosition(vec3 _position) { position = _position; };
	void setPosition(float x, float y, float z) 
	{ 
		position.x = x;
		position.y = y;
		position.z = z;
	};
	vec3& getPosition() { return position; };

	void setVelocity(vec3 _velocity) { velocity = _velocity; };
	void setVelocity(float x, float y, float z)
	{
		velocity.x = x;
		velocity.y = y;
		velocity.z = z;
	};
	vec3& getVelocity() { return velocity; };

	void setVelocitySTD(vec3 _velocity_std) { velocity_std = _velocity_std; };
	void setVelocitySTD(float x, float y, float z)
	{
		velocity_std.x = x;
		velocity_std.y = y;
		velocity_std.z = z;
	};
	vec3& getVelocitySTD() { return velocity_std; };
	
	void setColour(vec4 _colour) { colour = _colour; };
	void setColour(uint8 r, uint8 g, uint8 b, uint a)
	{
		colour[0] = r;
		colour[1] = g;
		colour[2] = b;
		colour[3] = a;
	};
	vec4& getColour() { return colour; };

	vec3 sampleVelocity();

	void setSize(float _size) { size = _size; };
	float getSize() { return size; };

	void setLifetime(float _lifetime) { lifetime = _lifetime; };
	float getLifetime() { return lifetime; };
};

namespace Random
{
	static std::default_random_engine s_RandomEngine;
	static std::normal_distribution<float> s_Distribution;

	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine);
	}

	
};