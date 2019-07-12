#include "rendering.h"
#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"

void Renderer::clear() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setRenderingState(uint vao, uint drawCommandBuffer) {
	glUseProgram(program);
	
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glBindVertexArray(vao);

}

void Renderer::draw(glm::mat4 view) {
	glm::mat4 projection = glm::perspective(glm::radians(45.f), 1600.f / 900.f, 0.5f, 10000000.f);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection * view));
	glm::vec4 cameraPos = glm::inverse(view) * glm::vec4(0.0, 0.0, 0.0, 1.0);
	glUniform3fv(1, 1, glm::value_ptr(glm::vec3(cameraPos)));

	glDrawArraysIndirect(GL_TRIANGLES, 0);
}
