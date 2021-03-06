#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <deque>
#include <fstream>

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include "Sphere.h"

GLFWwindow* window;

using namespace glm;

// Particle values
const int maxParticles = 10000;
std::deque<Particle*> particles(maxParticles);

// Particle Emitter
ParticleEmitter particleEmitter;

// World values
float gravity_multiplier = 1.0f;
float lifetime = 5.0f;
float dampening = 1.0f;
float radius = 25.0f;
const vec3 gravity(0, -9.81, 0);

// GPU buffers
static GLfloat* particle_position_size_data = new GLfloat[maxParticles * 4];
static GLubyte* particle_color_data = new GLubyte[maxParticles * 4];

int main(void)
{
	std::ofstream myfile;

	// Initialise GLFW
	glfwInit();

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Snow globe", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	glewInit();
	

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.1f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("resources/Particle.vertexshader", "resources/Particle.fragmentshader");

	// Vertex shader
	GLuint CameraRight_worldspace_ID = glGetUniformLocation(programID, "cameraRight_worldspace");
	GLuint CameraUp_worldspace_ID = glGetUniformLocation(programID, "cameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

	// fragment shader
	GLuint TextureID = glGetUniformLocation(programID, "textureSampler");
	GLuint Texture = loadDDS("resources/diamond.DDS");


	// Fill particles queue
	for (int i = 0; i < maxParticles; i++) {
		particles[i] = new Particle();
		particles[i]->life = -1.0f;
	}


	// VBO for particle QUAD.
	static const GLfloat particle_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};

	GLuint particle_vertex_buffer;
	glGenBuffers(1, &particle_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertex_buffer_data), particle_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	// The VBO containing the colors of the particles
	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);


	Sphere sphere(0, 0, 0, 30);
	std::cout << "Controls : " << std::endl;
	std::cout << '-' * 100 << std::endl;
	std::cout << "Gravity : decrease (z), increase (x)" << std::endl;
	std::cout << "Size of sphere : decrease (o), increase (p)" << std::endl;
	std::cout << "Particle life time : decrease (k), increase (l)" << std::endl;
	std::cout << "Dampening : decrease (n), increase (m)" << std::endl;

	// Set particle emitter characteristics
	particleEmitter.setColour(255.0f, 255.0f, 255.0f, 255.0f / 4.0f); // White
	particleEmitter.setSize(0.5f);
	particleEmitter.setVelocity(1.0f, 1.0f, 1.0f);
	particleEmitter.setVelocitySTD(5.0f, 5.0f, 5.0f);
	particleEmitter.setPosition(0.0f, 0.0f, 0.0f);

	// Main Loop
	double lastTime = glfwGetTime();
	double delta_average = 0.0f;
	long long framecount = 0.0f;

	// Tracking position of particle
	const Particle* dataParticlePtr = particles[0];
	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		framecount++;
		// Change in time
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		delta_average += delta;
		lastTime = currentTime;

		// process inputs
		process_inputs();

		// World view matricies
		mat4 ProjectionMatrix = getProjectionMatrix();
		mat4 ViewMatrix = getViewMatrix();
		vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
		mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		// update particle characteristics
		particleEmitter.setLifetime(getParticleLifeTime());

		// update world characteristics
		sphere.setDampening(getParticleDampening());
		gravity_multiplier = getGravityMultiplier();

		sphere.setRadius(getSphereSize());

		// Generate 10 new particule each millisecond,
		int newparticles = (int)(delta * 10000.0);
		if (newparticles > (int)(0.016f * 10000.0))
			newparticles = (int)(0.016f * 10000.0);

		// Generate new particles
		for (int i = 0; i < newparticles; i++) {
			
			Particle* particle = particles.front();
			if (particle->life > 0)
				break;
			particles.pop_front();

			particle->life = particleEmitter.getLifetime();
			particle->pos = particleEmitter.getPosition();
			
			particle->velocity = particleEmitter.sampleVelocity();

			particle->colour = particleEmitter.getColour();

			particle->size = particleEmitter.getSize();
			particles.push_back(particle);
		}

		// Simulate all particles
		int particlesCount = 0;
		myfile << dataParticlePtr->pos[0] << "," << dataParticlePtr->pos[1] << "," << dataParticlePtr->pos[2] << std::endl;

		for (int i = 0; i < maxParticles; i++) {

			Particle* p = particles[i];

			if (p->life > 0.0f) {

				// Decrease life
				p->life -= delta;
				if (p->life > 0.0f) {

					// gravity
					p->velocity += gravity * (float)delta * gravity_multiplier;

					// collsions
					if (sphere <= *p)
					{
						sphere.bounce(*p, delta);
					}

					p->colour[3] -= delta;

					// update characteristics
					p->pos += p->velocity * (float)delta;
					

					// Fill the GPU buffer
					// positions
					particle_position_size_data[4 * particlesCount] = p->pos.x;
					particle_position_size_data[4 * particlesCount + 1] = p->pos.y;
					particle_position_size_data[4 * particlesCount + 2] = p->pos.z;

					// size
					particle_position_size_data[4 * particlesCount + 3] = p->size;

					// colour
					particle_color_data[4 * particlesCount] = p->colour[0];
					particle_color_data[4 * particlesCount + 1] = p->colour[1];
					particle_color_data[4 * particlesCount + 2] = p->colour[2];
					particle_color_data[4 * particlesCount + 3] = p->colour[3];

				}

				particlesCount++;

			}
		}


		// Update rendering buffers for DYNAMIC drawing
		// Update the buffers that OpenGL uses for rendering.

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); 
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, particle_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW); 
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, particle_color_data);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// Same as the particles tutorial
		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		// verticies of quads
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

		// particle positions
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0 );

		// particle colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0 );

		glVertexAttribDivisor(1, 1); // positions 
		glVertexAttribDivisor(2, 1); // color 

		// Draw particles
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&  glfwWindowShouldClose(window) == 0);

	// Cleanup
	delete[] particle_position_size_data;
	for (auto p : particles)
	{
		delete p;
	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &particle_vertex_buffer);
	glDeleteProgram(programID);

	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
	std::cout << "Average framerate : " << delta_average / (float)framecount << std::endl;
	glfwTerminate();


	return 0;
}

