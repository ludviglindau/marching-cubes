#include "vertexgen.h"
#include "../glew/include/GL/glew.h"
#include "../glm/ext/vector_float4.hpp"
#include "tables.cpp"

void VertexGenShader::draw()
{
	glUseProgram(program);
	// reset counter
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	uint data = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint), &data, GL_DYNAMIC_DRAW);

	glClearNamedBufferData(vertexBuffer, GL_RGBA32F, GL_RGBA, GL_FLOAT, NULL);
	
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

	glDispatchCompute(8, 8, 8);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void VertexGenShader::genBuffers()
{
	uint index = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	uint triangleSize = sizeof(glm::vec4) * 6;
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxels * triangleSize * 10, nullptr, GL_DYNAMIC_DRAW);
	index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "TriangleBuffer");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, vertexBuffer);

	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
	uint data = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint), &data, GL_DYNAMIC_DRAW);
	index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Counter");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, counterBuffer);

	glGenBuffers(1, &edgeTableBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeTableBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256, &edgeTable, GL_STATIC_READ);
	index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "EdgeTable");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, edgeTableBuffer);

	glGenBuffers(1, &triangleTableBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleTableBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256 * 16, &triTable, GL_STATIC_READ);
	index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "TriangleTable");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, triangleTableBuffer);

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*)(sizeof(glm::vec4)));
}
