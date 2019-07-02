#include "pch.h"
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

// VAO and VBO will store IDs once generated (actual data on graphics card)
GLuint VAO, VBO, shader;

// Vertex shader
static const char* vShader = "													\n\
																				\n\
#version 330																	\n\
layout (location = 0) in vec3 pos;												\n\
																				\n\
void main()																		\n\
{																				\n\
	gl_Position = vec4(0.5 * pos.x, 0.5 * pos.y, 0.5 * pos.z, 1.0);				\n\
}";

// Fragment shader
static const char* fShader = "					\n\
#version 330									\n\
												\n\
out vec4 colour;								\n\
												\n\
void main()										\n\
{												\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);			\n\
}";

void CreateTriangle()
{
	// Locations of vertices
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Create VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Connect vertices to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Static means values won't change


	glVertexAttribPointer(
		0, // Layout index
		3, // 3 values (xyz)
		GL_FLOAT, // value type
		GL_FALSE, // Don't normalise values
		0, // Don't skip any values
		0); // No offset

	glEnableVertexAttribArray(0);

	// Unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	// Create and get ID
	GLuint theShader = glCreateShader(shaderType);

	// Pointer to first element of array
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	// Set up source code
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	// Check compile
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	// Attach shader
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		printf("Error creating shader program!\n");
		return;
	}

	// Set up shaders
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	// Initialise to store error information
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Create executables on graphics card
	glLinkProgram(shader);

	// Check link
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	// Check the shader is valid in current context
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

}

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!\n");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // These two lines set version to 3.3 (major.minor)
	// Won't be backwards compatable
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!\n");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features;
	glewExperimental = GL_TRUE;

	// Initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!\n");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Create viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// Loop until window closed

	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw triangle
		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}

