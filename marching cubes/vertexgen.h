#pragma once
#include "../glm/glm.hpp"

typedef unsigned int uint;

class VertexGenShader {
private:
public:
	uint voxelDim;
	uint program;
	uint noiseTexture;
	uint vertexBuffer;
	uint counterBuffer;
	uint edgeTableBuffer;
	uint triangleTableBuffer;
	uint vertexArray;
	void draw(glm::ivec3 chunk);
	void createBuffers();
	void destroyBuffers();
	uint getNumberOfTriangles();
};
