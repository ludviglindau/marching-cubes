#pragma once
#include "../glm/glm.hpp"

typedef unsigned int uint;

class Renderer {
private:
public:
	uint program;
	void clear();
	void setRenderingState(uint vao, uint drawCommandBuffer);
	void draw(glm::mat4 view);
};
