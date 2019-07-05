#pragma once
#include "../glm/glm.hpp"

typedef unsigned int uint;

class NoiseShader {
private:
public:
	const int TEXTURE_SIZE = 33;
	uint program;
	uint texture;
	float seed = 0.0f;
	void draw(glm::ivec3 chunk);
	void createTexture();
	void destroyTexture();
};
