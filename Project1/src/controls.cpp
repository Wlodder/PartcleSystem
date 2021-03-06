
#include <algorithm>
#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
float gravityMultiplier = 0.05f;
float gravityMultiplierDelta = 0.05f;
float dampeningMultiplier = 0.9f;
float dampeningMultiplierDelta = 0.1f;
float sphereSize = 25.0f;
float sphereSizeDelta = 1.0f;
float particleLifetime = 5.0f;
float particleLifetimeDelta = 0.05f;

glm::mat4 getViewMatrix()
{
	return ViewMatrix;
}

glm::mat4 getProjectionMatrix()
{
	return ProjectionMatrix;
}

float getGravityMultiplier()
{
	return gravityMultiplier;
}

float getSphereSize()
{
	return sphereSize;
};

float getParticleLifeTime()
{
	return particleLifetime;
};
float getParticleDampening()
{
	return dampeningMultiplier;
};


// camera initial position
glm::vec3 position = glm::vec3( 0, 0, 50 ); 

// initial horizontal angle
float horizontalAngle = 3.14f;

// initial vertical angle
float verticalAngle = 0.0f;

// initial field of view
float initialFoV = 45.0f;

// camera properties
float cameraSpeed = 6.0f; 
float mouseSpeed = 0.001f;



// exercise 2 
void process_inputs(){

	static double lastTime = glfwGetTime();

	// time frame difference
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// longitude and latitiude to direction
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// right vector for strafing
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - glm::half_pi<float>()),
		0,
		cos(horizontalAngle - glm::half_pi<float>())
	);
	
	// up vector
	glm::vec3 up = glm::cross( right, direction );

	// move forward in camera direction
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * cameraSpeed;
	}
	// move backward in camera direction
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * cameraSpeed;
	}
	// strafe right (perpendicular to camera diretion)
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * cameraSpeed;
	}
	// strafe left (perpendicular to camera diretion)
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * cameraSpeed;
	}

	// change gravity
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		gravityMultiplier += gravityMultiplierDelta;
	}
	else if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		gravityMultiplier -= gravityMultiplierDelta;
	}

	// change sphere size
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		sphereSize += sphereSizeDelta;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		sphereSize -= sphereSizeDelta;
	}

	// change particle life time
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		particleLifetime += particleLifetimeDelta;
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		particleLifetime -= particleLifetimeDelta;
		particleLifetime = std::max(0.0f, particleLifetime);
	}

	// change particle collision dampening
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		dampeningMultiplier += dampeningMultiplierDelta;
	}
	else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		dampeningMultiplier -= dampeningMultiplierDelta;
		dampeningMultiplier = std::max(0.0f, dampeningMultiplier);
	}

	float FoV = initialFoV;

	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 200.0f);

	// camera matrix
	ViewMatrix = glm::lookAt(position, position+direction, up);

	lastTime = currentTime;
}