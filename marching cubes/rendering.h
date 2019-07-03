#pragma once
#include "../glm/glm.hpp"

typedef unsigned int uint;

class Renderer {
private:
public:
	uint program;
	uint vbo;
	uint triangles;
	glm::vec3 translate = glm::vec3(-16, -16 * 2.5, -16 * 8.0);
	glm::vec3 scale = glm::vec3(2.0, 2.0, 2.0);
	glm::vec3 rotationAxis = glm::vec3(0.0, 1.0, 0.0);
	float rotationAngle = 0.0;
	void init();
	void draw(uint vao);
};
