#include "rendering.h"
#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"

void Renderer::clear() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(uint vao, glm::mat4 world, glm::mat4 view)
{
	glUseProgram(program);
	
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection = glm::perspective(glm::radians(45.f), 1600.f / 900.f, 0.5f, 10000.f);
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection * view));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
}
