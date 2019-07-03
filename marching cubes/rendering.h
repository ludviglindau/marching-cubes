#pragma once
#include "../glm/glm.hpp"

typedef unsigned int uint;

class Renderer {
private:
public:
	uint program;
	uint triangles;
	void draw(uint vao, glm::mat4 world, glm::mat4 view);
};
