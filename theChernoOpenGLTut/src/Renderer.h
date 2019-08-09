#pragma once

#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

/* Call all Error Messages out of the Error Stack */
void GLClearError();

/* Print Error Messages to the console */
// To refference error numbers,
// you will have to convert to hexadecimal
bool GLLogCall(const char* function, const char* file, int line);