#include "Renderer.h"

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

/* Print Error Messages to the console */
// To refference error numbers,
// you will have to convert to hexadecimal
bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): "
			<< function << " "
			<< file << ":"
			<< line << std::endl;
		return false;
	}
	return true;
}