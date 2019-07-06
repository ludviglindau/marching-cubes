#include "rendering.h"
#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"

void Renderer::clear() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(uint vao, glm::mat4 view) {
	glUseProgram(program);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.f), 1600.f / 900.f, 0.5f, 10000.f);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection * view));
	glm::vec4 cameraPos = glm::inverse(view) * glm::vec4(0.0, 0.0, 0.0, 1.0);
	glUniform3fv(1, 1, glm::value_ptr(glm::vec3(cameraPos)));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
}
