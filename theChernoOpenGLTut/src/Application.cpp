#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

/* Read Shader From File */
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	/* Create in-stream object */
	std::ifstream stream(filepath);

	/* Specify Function Modes */
	enum class ShaderType
	{
		NONE		= -1,
		VERTEX		=  0,
		FRAGMENT	=  1,
	};
	ShaderType type = ShaderType::NONE; //initialize function mode

	/* Allocate Data */
	std::string line;
	std::stringstream ss[2];

	/* Read File */
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

/* Compile a shader from a string of source code */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	/* Create id for shader storage */
	GLCall(unsigned int id = glCreateShader(type));
	
	/* Obtain pure string location */
	const char* src = source.c_str();

	/* load source code into OpenGL */
	GLCall(glShaderSource(id, 1, &src, nullptr));

	/* Compile Shader */
	GLCall(glCompileShader(id));

	/* Check for Compiler Errors */
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	/* Handle Errors */
	if (result == GL_FALSE)
	{
		/* Read in Error Message */
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char)); //alloca is a function for allocating memory on the stack
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		
		/* Print Error */
		std::cout	<< "failed to compile "
					<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
					<< " shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	/* Return index for future use */
	return id;
}

/* Compile an OpenGL program from shader source code */
static unsigned int CreateShader(const std::string& vertexShader, const std::string & fragmentShader)
{
	/* Create ids for shader components */
	GLCall(unsigned int program = glCreateProgram());								//Final Shader Program
	GLCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));		//vertex Shader
	GLCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));	//fragment Shader

	/* Associate Shaders with program */
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	/* Link Shaders together */
	GLCall(glLinkProgram(program));

	/* Validate Program */
	GLCall(glValidateProgram(program));

	/* Clear Temporary files */
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	/* return program id */
	return program;
}

static void Bounce(float& colour, float& increment)
{
	if (colour > 1.0f || colour < 0.0f)
		increment = -increment;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Specify OpenGL Version and Mode */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Lock Framerate */
	glfwSwapInterval(1);

	/* Check for errors in glewinit before proceeding */
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR! GLEW IS NOT OKAY" << std::endl;

	/* output OpenGL Version Number in use */
	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		/* Load or Create Graphical Data */
		float imageData[] =
		{//		x		y
			 -0.5f,	-0.5f,	// point 1
			  0.5f,	-0.5f,	// point 2
			  0.5f,	 0.5f,	// point 3
			 -0.5f,	 0.5f,	// point 4
		};

		unsigned int imageIndex[] =
		{
			0, 1, 2,		// triangle 1
			2, 3, 0,		// triangle 2
		};

		/* Create OpenGL context to use data */
		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexBuffer vb(imageData, 4 * 2 * sizeof(float));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer							//Contextualize buffer data
		(
			0,					//Index number of attribute to be specified
			2,					//number of data points per attribute
			GL_FLOAT,			//data type of attribute
			GL_FALSE,			//normalize data (yes/no)
			sizeof(float) * 2,	//distance from vertex to vertex
			0					//distance of attribute from the beginning of the vertex
		));

		IndexBuffer ib(imageIndex, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

		/* Compile Shader */
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

		/* Activate Shader */
		GLCall(glUseProgram(shader));

		/* Set Uniform Data */
		// Must be done after a shader is attached
		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

		float
			r = 0.0f,
			g = 0.0f,
			b = 0.0f,
			incR = 0.05f,
			incG = 0.04f,
			incB = 0.03f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Reset Frame Buffer */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			/* Render here */
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, g, b, 1.0f));

			GLCall(glBindVertexArray(vao));
			ib.Bind();

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			Bounce(r, incR);
			Bounce(g, incG);
			Bounce(b, incB);
			r += incR;
			g += incG;
			b += incB;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		/* Free Used Memory */
		GLCall(glDeleteProgram(shader));
	}

	/* Shutdown GLFW and exit program */
	glfwTerminate();
	return 0;
}