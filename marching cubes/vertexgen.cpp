#include "vertexgen.h"
#include "../glew/include/GL/glew.h"
#include "../glm/gtc/type_ptr.hpp"
#include "tables.cpp"

void VertexGenShader::draw(glm::ivec3 chunk)
{
	glUseProgram(program);
	// reset counter
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	uint data = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint), &data, GL_STATIC_DRAW);

	ChunkRenderInfo& chunkInfo = chunkRenderInfos[chunk];
	if (chunkInfo.invalid) {
		chunkInfo.deleteBuffers();
		chunkInfo.createBuffers(voxelDim);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkInfo.vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunkInfo.vertexBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, counterBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeTableBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, edgeTableBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleTableBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, triangleTableBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, drawCommandBuffer);

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);

	glUniform1f(0, voxelDim);
	glUniform3iv(1, 1, glm::value_ptr(chunk));
	glUniform1f(2, scale);

	glDispatchCompute(voxelDim / 8, voxelDim / 8, voxelDim / 8);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	if (chunkInfo.triangleCount == 0) {
		glGetNamedBufferSubData(counterBuffer, 0, sizeof(uint), &chunkInfo.triangleCount);
		chunkInfo.moveToBuffer(bigVertexBuffer, bigVertexBufferSize);
	}
}

void VertexGenShader::createBuffers()
{
	uint index = 0;
	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	uint data = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint), &data, GL_DYNAMIC_DRAW);
	index = 1;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, counterBuffer);

	glGenBuffers(1, &edgeTableBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeTableBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256, &edgeTable, GL_STATIC_READ);
	index = 2;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, edgeTableBuffer);

	glGenBuffers(1, &triangleTableBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleTableBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256 * 16, &triTable, GL_STATIC_READ);
	index = 3;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, triangleTableBuffer);

	glGenBuffers(1, &bigVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bigVertexBuffer);
	bigVertexBufferLength = voxelDim * voxelDim * voxelDim * 5 * 50 * triangleSize;
	glBufferStorage(GL_ARRAY_BUFFER, bigVertexBufferLength, 0, 0);

	glGenVertexArrays(1, &bigVertexArray);
	glBindVertexArray(bigVertexArray);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(glm::vec4) * 2, (void*)(sizeof(glm::vec4)));

	glGenBuffers(1, &drawCommandBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(uint) * 4, 0, 0);
}

void VertexGenShader::destroyBuffers() {
	glDeleteBuffers(1, &counterBuffer);
	glDeleteBuffers(1, &edgeTableBuffer);
	glDeleteBuffers(1, &triangleTableBuffer);
}

void VertexGenShader::clear() {
	chunkRenderInfos = std::unordered_map<glm::ivec3, ChunkRenderInfo, KeyHash>();
	bigVertexBufferSize = 0;
	drawCommandIndex = 0;
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(uint) * 4, 0, 0);
}

void ChunkRenderInfo::createBuffers(uint voxelDim) {
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	
	glBufferData(GL_ARRAY_BUFFER, voxelDim * voxelDim * voxelDim * triangleSize * 5, nullptr, GL_STATIC_COPY);

	invalid = false;
}

void ChunkRenderInfo::moveToBuffer(uint targetBuffer, uint& targetBufferSize) {
	glCopyNamedBufferSubData(vertexBuffer, targetBuffer, 0, targetBufferSize, triangleCount * triangleSize);
	
	targetBufferSize += triangleCount * triangleSize;
	glDeleteBuffers(1, &vertexBuffer);

	invalid = false;
}

void ChunkRenderInfo::deleteBuffers() {
	if (vertexBuffer < UINT_MAX) {
		glDeleteBuffers(1, &vertexBuffer);
	}
	triangleCount = 0;
	invalid = true;
}

ChunkRenderInfo::~ChunkRenderInfo() {
	deleteBuffers();
}
