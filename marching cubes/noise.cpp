#include "Noise.h"
#include "shader.h"
#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"
#include <stdlib.h>
#include "../glm/gtc/type_ptr.hpp"

void NoiseShader::draw(glm::ivec3 chunk)
{
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUniform1f(0, seed);
	glUniform1f(1, glfwGetTime());
	glUniform1f(2, TEXTURE_SIZE);
	glUniform3iv(3, 1, glm::value_ptr(chunk));
	
	glDispatchCompute(TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void NoiseShader::createTexture()
{
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
}

void NoiseShader::destroyTexture()
{
	glDeleteTextures(1, &texture);
}
