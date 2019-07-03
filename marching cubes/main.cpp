#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"

#include "../glm/glm.hpp"
#include "../glm/ext.hpp"

#include "shader.h"
#include "Noise.h"
#include "vertexgen.h"
#include "rendering.h"

#include <stdio.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

typedef uint32_t uint;

GLFWwindow* window;

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

void initWindow();


void GLAPIENTRY
MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

int main()
{
	initWindow();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	

	NoiseShader noiseShader;
	createComputeShader(noiseShader.program, "noise.glsl");
	noiseShader.genTexture();
	glfwPollEvents();
	noiseShader.draw();
	
	VertexGenShader vertexGenShader;
	vertexGenShader.noiseTexture = noiseShader.texture;
	createComputeShader(vertexGenShader.program, "vertexGen.glsl");
	vertexGenShader.genBuffers();
	vertexGenShader.draw();

	Renderer renderer;
	createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");
	renderer.triangles = vertexGenShader.voxels * sizeof(glm::vec3) * 6 * 10;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			createComputeShader(noiseShader.program, "noise.glsl");

			createComputeShader(vertexGenShader.program, "vertexGen.glsl");

			createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			renderer.rotationAngle -= 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			renderer.rotationAngle += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			renderer.scale += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			renderer.scale -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		noiseShader.draw();
		vertexGenShader.draw();

		renderer.draw(vertexGenShader.vertexArray);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void initWindow()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cubes", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glfwSwapInterval(1);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(err));
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	fprintf(stderr, "Renderer: %s\n", renderer);
	fprintf(stderr, "OpenGL version %s\n", version);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClearDepth(1.0f);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}