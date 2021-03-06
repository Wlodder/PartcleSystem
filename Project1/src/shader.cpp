#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <glew.h>

#include "shader.hpp"

// partial code from https://learnopengl.com/Getting-started/Shaders &&  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/

GLuint LoadShaders(const string& vertex_file_path,const string& fragment_file_path){

	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// read vertex shader
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
	if(vertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		vertexShaderStream.close();
	}

	// read fragment shader
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		fragmentShaderStream.close();
	}

	// Compile vertex Shader
	std::cout << "Compiling shader : " << vertex_file_path << std::endl;
	char const * vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
	glCompileShader(vertexShaderID);

	// Compile fragment Shader
	std::cout << "Compiling shader : " << fragment_file_path << std::endl;
	char const * fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
	glCompileShader(fragmentShaderID);

	// Link the program
	std::cout << "Linking program" << std::endl;
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}


