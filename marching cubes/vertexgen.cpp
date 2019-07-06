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

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, counterBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeTableBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, edgeTableBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleTableBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, triangleTableBuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);

	glUniform1f(0, voxelDim);
	glUniform3iv(1, 1, glm::value_ptr(chunk));
	glUniform1f(2, scale);

	glDispatchCompute(voxelDim / 8, voxelDim / 8, voxelDim / 8);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	if (triangleCounts.count(chunk) == 0) {
		glGetNamedBufferSubData(counterBuffer, 0, sizeof(uint), &nrOfTriangles);
		triangleCounts[chunk] = nrOfTriangles;
	}
}

void VertexGenShader::createBuffers()
{
	uint index = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	uint triangleSize = sizeof(glm::vec4) * 6;
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelDim * voxelDim * voxelDim * triangleSize * 5, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, vertexBuffer);

	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	uint data = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint), &data, GL_STATIC_DRAW);
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

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 0);

	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(glm::vec4) * 2, (void*)(sizeof(glm::vec4)));
}

void VertexGenShader::destroyBuffers() {
	glDeleteBuffers(1, &counterBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &edgeTableBuffer);
	glDeleteBuffers(1, &triangleTableBuffer);
	glDeleteVertexArrays(1, &vertexArray);
}

void ChunkRenderInfo::createVertexBuffer(uint voxelDim) {
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	uint triangleSize = sizeof(glm::vec4) * 6;
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelDim * voxelDim * voxelDim * triangleSize * 5, nullptr, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(glm::vec4) * 2, (void*)(sizeof(glm::vec4)));
}

ChunkRenderInfo::~ChunkRenderInfo() {
	if (vertexBuffer < UINT_MAX) {
		glDeleteBuffers(1, &vertexBuffer);
	}
	if (vertexArray < UINT_MAX) {
		glDeleteVertexArrays(1, &vertexArray);
	}
}
