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

bool wireframeMode = false;

struct Camera {
	glm::vec2 rotation = glm::vec2(0.0f, 0.0f);
	float distance = 100.0f;
} camera;


glm::vec3 translate;
glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 rotationAxis = glm::vec3(0.0, 1.0, 0.0);
float rotationAngle = 0.f;
glm::mat4 world = glm::mat4(1.0);
glm::mat4 view = glm::mat4(1.0);
glm::mat4 projection = glm::perspective(3.14159f, 16.f / 9.f, 0.1f, 1000.0f);

void initWindow();
void updateWorldMatrix(glm::vec3 chunkLength);
void updateViewMatrix();
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	
	NoiseShader noiseShader;
	createComputeShader(noiseShader.program, "noise.glsl");
	noiseShader.createTexture();
	glfwPollEvents();
	noiseShader.draw(glm::vec3(0.0, 0.0, 0.0));
	
	VertexGenShader vertexGenShader;
	vertexGenShader.noiseTexture = noiseShader.texture;
	createComputeShader(vertexGenShader.program, "vertexGen.glsl");
	vertexGenShader.voxelDim = (noiseShader.TEXTURE_SIZE - 1);
	vertexGenShader.createBuffers();
	vertexGenShader.draw(glm::vec3(0.0, 0.0, 0.0));

	noiseShader.draw(glm::vec3(0.0, -1.0, 0.0));
	VertexGenShader vertexGenShader2;
	vertexGenShader2.noiseTexture = noiseShader.texture;
	vertexGenShader2.program = vertexGenShader.program;
	vertexGenShader2.voxelDim = (noiseShader.TEXTURE_SIZE - 1);
	vertexGenShader2.createBuffers();
	vertexGenShader2.draw(glm::vec3(0.0, -1.0, 0.0));

	Renderer renderer;
	createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");
	renderer.triangles = vertexGenShader.getNumberOfTriangles();
	translate = glm::vec3(0.0);	
	view = glm::lookAt(glm::vec3(0.0, 0.0, 100.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));

	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			createComputeShader(noiseShader.program, "noise.glsl");

			createComputeShader(vertexGenShader.program, "vertexGen.glsl");
			vertexGenShader2.program = vertexGenShader.program;

			createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");
			
			noiseShader.draw(glm::vec3(0.0, 0.0, 0.0));
			vertexGenShader.draw(glm::vec3(0.0, 0.0, 0.0));
			noiseShader.draw(glm::vec3(0.0, -1.0, 0.0));
			vertexGenShader2.draw(glm::vec3(0.0, -1.0, 0.0));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			camera.rotation.x -= 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			camera.rotation.x += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			camera.rotation.y = glm::clamp(camera.rotation.y + 0.01f, -1.62f, 1.62f);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			camera.rotation.y = glm::clamp(camera.rotation.y - 0.01f, -1.62f, 1.62f);
		}
		if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
			camera.distance += 0.5f;
		}
		if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
			camera.distance -= 0.5f;
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			wireframeMode = !wireframeMode;
			if (wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		renderer.clear();

		//noiseShader.draw(glm::vec3(0.0, 0.0, 0.0));
		//vertexGenShader.draw();
		translate = glm::vec3(0.0);
		updateWorldMatrix(glm::vec3(vertexGenShader.voxelDim));
		updateViewMatrix();
		renderer.triangles = vertexGenShader.getNumberOfTriangles();
		renderer.draw(vertexGenShader.vertexArray, world, view);


		//noiseShader.draw(glm::vec3(0.0, -1.0, 0.0));
		//vertexGenShader.draw();
		translate = glm::vec3(0.0, -(noiseShader.TEXTURE_SIZE - 1), 0.0) * scale;
		renderer.triangles = vertexGenShader2.getNumberOfTriangles();
		updateWorldMatrix(glm::vec3(vertexGenShader2.voxelDim));
		renderer.draw(vertexGenShader2.vertexArray, world, view);


		glfwSwapBuffers(window);
	}
	
	noiseShader.destroyTexture();
	vertexGenShader.destroyBuffers();
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
	glfwSwapInterval(0);

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

void updateWorldMatrix(glm::vec3 chunkLength) {
	world = glm::mat4(1.0);
	world = glm::translate(world, translate);
	world = glm::scale(world, scale);

	world = glm::translate(world, chunkLength * 0.5f);
	world = glm::rotate(world, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	world = glm::translate(world, -chunkLength * 0.5f);
}

void updateViewMatrix() {
	view = glm::mat4(1.0);
	view = glm::translate(view, glm::vec3(0.0, 0.0, -camera.distance));
	view = glm::rotate(view, camera.rotation.y, glm::vec3(1.0, 0.0, 0.0));
	view = glm::rotate(view, camera.rotation.x, glm::vec3(0.0, 1.0, 0.0));
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