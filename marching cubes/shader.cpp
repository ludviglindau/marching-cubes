#include "shader.h"
#include "../glew/include/GL/glew.h"
#include "../glfw/include/GLFW/glfw3.h"
#include <stdio.h>
#include <string.h>

void createShaderProgram(uint& shaderProgram, const char* VSfileName, const char* TCSfileName, const char* TESfileName, const char* GSfileName, const char* FSfileName) {
	char errorBuffer[1024];
	memset(errorBuffer, 0, 1024);
	GLint compileResult;
	const int BUFFER_SIZE = 4096 * 2;
	char* textBuffer = new char[BUFFER_SIZE];
	memset(textBuffer, 0, BUFFER_SIZE);
	uint VS = 0;
	uint TES = 0;
	uint TCS = 0;
	uint GS = 0;
	uint FS = 0;
	FILE* VSfile = nullptr;
	FILE* TESfile = nullptr;
	FILE* TCSfile = nullptr;
	FILE* GSfile = nullptr;
	FILE* FSfile = nullptr;
	errno_t err = 0;
	int charsRead = 0;

	//create vertex shader
	VS = glCreateShader(GL_VERTEX_SHADER);

	err = fopen_s(&VSfile, VSfileName, "r");

	if (err) {
		fprintf(stderr, "error while reading %s, errno %i\n", VSfileName, err);
		return;
	}

	charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, VSfile);

	glShaderSource(VS, 1, &textBuffer, &charsRead);
	glCompileShader(VS);
	compileResult = GL_FALSE;
	glGetShaderiv(VS, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(VS, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	fclose(VSfile);

	//create tesselation evaluation shader
	if (TESfileName != nullptr)
	{
		TES = glCreateShader(GL_TESS_EVALUATION_SHADER);

		err = fopen_s(&TESfile, TESfileName, "r");
		if (err) {
			fprintf(stderr, "error while reading %s, errno %i\n", TESfileName, err);
			return;
		}

		memset(textBuffer, 0, BUFFER_SIZE);
		charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, TESfile);

		glShaderSource(TES, 1, &textBuffer, &charsRead);
		glCompileShader(TES);
		compileResult = GL_FALSE;
		glGetShaderiv(TES, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			glGetShaderInfoLog(TES, 1024, nullptr, errorBuffer);
			fprintf(stderr, "%s\n", errorBuffer);
		}

		fclose(TESfile);
	}

	//create tesselation control shader
	if (TCSfileName != nullptr)
	{
		TCS = glCreateShader(GL_TESS_CONTROL_SHADER);

		err = fopen_s(&TCSfile, TCSfileName, "r");
		if (err) {
			fprintf(stderr, "error while reading %s, errno %i\n", TCSfileName, err);
			return;
		}

		memset(textBuffer, 0, BUFFER_SIZE);
		charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, TCSfile);

		glShaderSource(TCS, 1, &textBuffer, &charsRead);
		glCompileShader(TCS);
		compileResult = GL_FALSE;
		glGetShaderiv(TCS, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			glGetShaderInfoLog(TCS, 1024, nullptr, errorBuffer);
			fprintf(stderr, "%s\n", errorBuffer);
		}

		fclose(TCSfile);

	}

	//create geometry shader
	if (GSfileName != nullptr)
	{
		GS = glCreateShader(GL_GEOMETRY_SHADER);

		err = fopen_s(&GSfile, GSfileName, "r");
		if (err) {
			fprintf(stderr, "error while reading %s, errno %i\n", GSfileName, err);
			return;
		}

		memset(textBuffer, 0, BUFFER_SIZE);
		charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, GSfile);

		glShaderSource(GS, 1, &textBuffer, &charsRead);
		glCompileShader(GS);
		compileResult = GL_FALSE;
		glGetShaderiv(GS, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			glGetShaderInfoLog(GS, 1024, nullptr, errorBuffer);
			fprintf(stderr, "%s\n", errorBuffer);
		}

		fclose(GSfile);
	}

	//create fragment shader
	if (FSfileName != nullptr)
	{
		FS = glCreateShader(GL_FRAGMENT_SHADER);

		err = fopen_s(&FSfile, FSfileName, "r");
		if (err) {
			fprintf(stderr, "error while reading %s, errno %i\n", FSfileName, err);
			return;
		}

		memset(textBuffer, 0, BUFFER_SIZE);
		charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, FSfile);

		glShaderSource(FS, 1, &textBuffer, &charsRead);
		glCompileShader(FS);
		compileResult = GL_FALSE;
		glGetShaderiv(FS, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			glGetShaderInfoLog(FS, 1024, nullptr, errorBuffer);
			fprintf(stderr, "%s\n", errorBuffer);
		}

		fclose(FSfile);
	}

	//create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, VS);
	if (TESfileName != nullptr)
		glAttachShader(shaderProgram, TES);
	if (TCSfileName != nullptr)
		glAttachShader(shaderProgram, TCS);
	if (GSfileName != nullptr)
		glAttachShader(shaderProgram, GS);
	if (FSfileName != nullptr)
		glAttachShader(shaderProgram, FS);
	glLinkProgram(shaderProgram);

	GLint linkResult = GL_FALSE;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE) {
		glGetProgramInfoLog(shaderProgram, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	glDetachShader(shaderProgram, VS);
	glDeleteShader(VS);

	if (TESfileName != nullptr) {
		glDetachShader(shaderProgram, TES);
		glDeleteShader(TES);
	}

	if (TCSfileName != nullptr) {
		glDetachShader(shaderProgram, TCS);
		glDeleteShader(TCS);
	}

	if (GSfileName != nullptr) {
		glDetachShader(shaderProgram, GS);
		glDeleteShader(GS);
	}

	if (FSfileName != nullptr) {
		glDetachShader(shaderProgram, FS);
		glDeleteShader(FS);
	}

	delete[] textBuffer;
}

void createComputeShader(uint& shaderProgram, const char* CSfileName)
{
	char errorBuffer[1024];
	memset(errorBuffer, 0, 1024);
	GLint compileResult;
	const int BUFFER_SIZE = 4096 * 20;
	char* textBuffer = new char[BUFFER_SIZE];
	memset(textBuffer, 0, BUFFER_SIZE);
	uint CS = 0;
	FILE* CSfile = nullptr;
	errno_t err = 0;
	int charsRead = 0;

	//create vertex shader
	CS = glCreateShader(GL_COMPUTE_SHADER);

	err = fopen_s(&CSfile, CSfileName, "r");

	if (err) {
		fprintf(stderr, "error while reading %s, errno %i\n", CSfileName, err);
		return;
	}

	charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, CSfile);

	glShaderSource(CS, 1, &textBuffer, &charsRead);
	glCompileShader(CS);
	compileResult = GL_FALSE;
	glGetShaderiv(CS, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(CS, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	fclose(CSfile);

	//create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, CS);
	glLinkProgram(shaderProgram);

	GLint linkResult = GL_FALSE;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE) {
		glGetProgramInfoLog(shaderProgram, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	glDetachShader(shaderProgram, CS);
	glDeleteShader(CS);

	delete[] textBuffer;
}
