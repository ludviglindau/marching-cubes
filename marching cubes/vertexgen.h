#pragma once
#include "../glm/glm.hpp"
#include <unordered_map>

typedef unsigned int uint;

struct KeyHash {
	uint operator()(const glm::ivec3& vec) const
	{
		int hash = 0;
		hash = vec.x & 255;
		hash += (vec.y & 255) << 8;
		hash += (vec.z & 255) << 16;

		return hash;
	}
};

class ChunkRenderInfo {
private:
public:
	uint triangleCount = 0;
	uint vertexBuffer = UINT_MAX;
	uint vertexArray = UINT_MAX;
	bool invalid = true;
	void createBuffers(uint voxelDim);
	void resizeBuffer();
	void deleteBuffers();
	~ChunkRenderInfo();
};

class VertexGenShader {
private:
public:
	uint voxelDim;
	uint program;
	uint noiseTexture;
	uint counterBuffer;
	uint edgeTableBuffer;
	uint triangleTableBuffer;
	uint nrOfTriangles = 0;
	std::unordered_map<glm::ivec3, ChunkRenderInfo, KeyHash> chunkRenderInfos;
	float scale = 2.0;
	void draw(glm::ivec3 chunk);
	void createBuffers();
	void destroyBuffers();
};
