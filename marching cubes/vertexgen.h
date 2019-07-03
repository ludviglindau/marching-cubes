#pragma once

typedef unsigned int uint;

class VertexGenShader {
private:
public:
	uint voxels = 32 * 32;
	uint program;
	uint noiseTexture;
	uint vertexBuffer;
	uint counterBuffer;
	uint edgeTableBuffer;
	uint triangleTableBuffer;
	uint vertexArray;
	void draw();
	void genBuffers();
};
