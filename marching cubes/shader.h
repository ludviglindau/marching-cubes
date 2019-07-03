#pragma once

typedef unsigned int uint;

void createShaderProgram(
	uint& shaderProgram,
	const char* VSfileName,
	const char* TCSfileName,
	const char* TESfileName,
	const char* GSfileName,
	const char* FSfileName);

void createComputeShader(uint& shaderProgram, const char* CSfileName);
