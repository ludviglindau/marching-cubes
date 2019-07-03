#pragma once

typedef unsigned int uint;

class VertexGenShader {
private:
public:
	uint voxelRows;
	uint program;
	uint noiseTexture;
	uint vertexBuffer;
	uint counterBuffer;
	uint edgeTableBuffer;
	uint triangleTableBuffer;
	uint vertexArray;
	void draw();
	void createBuffers();
	void destroyBuffers();
	uint getNumberOfTriangles();
};
