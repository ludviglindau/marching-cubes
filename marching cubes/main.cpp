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
#include <vector>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

typedef uint32_t uint;

GLFWwindow* window;

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
const int chunkRows = 14;
const int chunkRowStart = -7;
const int chunkCols = 7;
const int chunkColStart = -3;

bool wireframeMode = false;

struct Camera {
	glm::vec2 rotation = glm::vec2(0.0f, 0.0f);
	float distance = 10000.0f;
} camera;

glm::mat4 view = glm::mat4(1.0);

void initWindow();
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

	std::vector<NoiseShader> noiseShaders;
	noiseShaders.resize(chunkCols * chunkRows*chunkRows);
	createComputeShader(noiseShaders[0].program, "noise.glsl");

	for (int col = 0; col < chunkCols; ++col) {
		for (int row = 0; row < chunkRows*chunkRows; ++row) {
			noiseShaders[(col * (chunkRows*chunkRows)) + row].program = noiseShaders[0].program;
			noiseShaders[(col * (chunkRows*chunkRows)) + row].createTexture();
			noiseShaders[(col * (chunkRows*chunkRows)) + row].draw(glm::vec3((row % chunkRows) + chunkRowStart, chunkColStart + col, (row / chunkRows) + chunkRowStart));
		}
	}
	
	VertexGenShader vertexGenShader;
	createComputeShader(vertexGenShader.program, "vertexGen.glsl");
	vertexGenShader.voxelDim = (NoiseShader::TEXTURE_SIZE - 1);
	vertexGenShader.createBuffers();
	vertexGenShader.scale = 40.0;

	Renderer renderer;
	createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");
	view = glm::lookAt(glm::vec3(0.0, 0.0, 100.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));

	
	while (!glfwWindowShouldClose(window))
	{
		double dt = glfwGetTime();
		glfwSetTime(0.0);
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			createComputeShader(noiseShaders[0].program, "noise.glsl");

			for (int col = 0; col < chunkCols; ++col) {
				for (int row = 0; row < chunkRows*chunkRows; ++row) {
					noiseShaders[(col * (chunkRows*chunkRows)) + row].program = noiseShaders[0].program;
					noiseShaders[(col * (chunkRows*chunkRows)) + row].draw(glm::vec3((row % chunkRows) + chunkRowStart, chunkColStart + col, (row / chunkRows) + chunkRowStart));
				}
			}
			vertexGenShader.clear();
			createComputeShader(vertexGenShader.program, "vertexGen.glsl");

			createShaderProgram(renderer.program, "vs.glsl", nullptr, nullptr, nullptr, "fs.glsl");			
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			camera.rotation.x -= 1.0f * dt;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			camera.rotation.x += 1.0f * dt;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			camera.rotation.y = glm::clamp(camera.rotation.y + (1.f * (float)dt), -1.62f, 1.62f);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			camera.rotation.y = glm::clamp(camera.rotation.y - (1.f * (float)dt), -1.62f, 1.62f);
		}
		if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
			camera.distance += 150.5f * dt;
		}
		if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
			camera.distance -= 150.5f * dt;
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
		updateViewMatrix();
		bool dirty = false;

		for (int col = 0; col < chunkCols; ++col) {
			for (int row = 0; row < chunkRows*chunkRows; row++) {
				glm::ivec3 chunk = glm::ivec3((row % chunkRows) + chunkRowStart, chunkColStart + col, (row / chunkRows) + chunkRowStart);
				ChunkRenderInfo& chunkRenderInfo = vertexGenShader.chunkRenderInfos[chunk];
				if (chunkRenderInfo.invalid) {
					vertexGenShader.noiseTexture = noiseShaders[(col * (chunkRows*chunkRows)) + row].texture;
					vertexGenShader.draw(chunk);
					//noiseShaders[(col * (chunkRows*chunkRows)) + row].destroyTexture();
					dirty = true;
				}
			}
		}
		if (dirty) {
			renderer.setRenderingState(vertexGenShader.bigVertexArray, vertexGenShader.drawCommandBuffer);
		}
		renderer.draw(view);

		glfwSwapBuffers(window);
		char title[80];
		sprintf_s(title, 80, "%.3f ms, %i triangles", glfwGetTime()*1000, vertexGenShader.bigVertexBufferSize / triangleSize);
		glfwSetWindowTitle(window, title);
	}
	
	for (int i = 0; i < noiseShaders.size(); i++) {
		noiseShaders[i].destroyTexture();
	}
	vertexGenShader.destroyBuffers();
	glfwTerminate();
	return 0;
}

void initWindow()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif
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