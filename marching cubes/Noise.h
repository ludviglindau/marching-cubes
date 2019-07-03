#pragma once

typedef unsigned int uint;


class NoiseShader {
private:
public:
	const int TEXTURE_SIZE = 65;
	uint program;
	uint texture;
	float seed = 0.0f;
	void draw();
	void createTexture();
	void destroyTexture();
};
