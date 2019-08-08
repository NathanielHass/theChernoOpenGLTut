#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

/* Compile a shader from a string of source code */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	/* Create id for shader storage */
	unsigned int id = glCreateShader(type);
	
	/* Obtain pure string location */
	const char* src = source.c_str();

	/* load source code into OpenGL */
	glShaderSource(id, 1, &src, nullptr);

	/* Compile Shader */
	glCompileShader(id);

	/* Check for Compiler Errors */
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		/* Read in Error Message */
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char)); //alloca is a function for allocating memory on the stack
		glGetShaderInfoLog(id, length, &length, message);
		
		/* Print Error */
		std::cout	<< "failed to compile "
					<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
					<< " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	/* Return index for future use */
	return id;
}

/* Compile an OpenGL program from shader source code */
static unsigned int CreateShader(const std::string& vertexShader, const std::string & fragmentShader)
{
	/* Create ids for shader components */
	unsigned int program = glCreateProgram();								//Final Shader Program
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);		//vertex Shader
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);	//fragment Shader

	/* Associate Shaders with program */
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	/* Link Shaders together */
	glLinkProgram(program);

	/* Validate Program */
	glValidateProgram(program);

	/* Clear Temporary files */
	glDeleteShader(vs);
	glDeleteShader(fs);

	/* return program id */
	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Check for errors in glewinit before proceeding */
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR! GLEW IS NOT OKAY" << std::endl;

	/* output OpenGL Version Number in use */
	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Load or Create Graphical Data */
	float imageData[6] =
	{//		x		y
		 -0.5f,	-0.5f,	// point 1
		 -0.0f,	 0.5f,	// point 2
		  0.5f,	-0.5f,	// point 3
	};

	/* Create OpenGL context to use data */
	unsigned int vertexBuffer;						//create ID tag
	glGenBuffers(1, &vertexBuffer);					//assign memory to tag
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	//switch OpenGL state to specified buffer

	glVertexAttribPointer							//Contextualize buffer data
	(
		0,					//Index number of attribute to be specified
		2,					//number of data points per attribute
		GL_FLOAT,			//data type of attribute
		GL_FALSE,			//normalize data (yes/no)
		sizeof(float) * 2,	//distance from vertex to vertex
		0					//distance of attribute from the beginning of the vertex
	);
	glEnableVertexAttribArray(0);					//Enable Specified Attrib Array

	/* Load Data into OpenGL */
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), imageData, GL_STATIC_DRAW);

	/* Clear Bindings after use */
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Load or Create Shader Data */
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n";
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

	/* Compile Shader */
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	
	/* Activate Shader */
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);				//reset framebuffer
		
		/* Render here */
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	/* Free Used Memory */
	glDeleteProgram(shader);

	/* Shutdown GLFW and exit program */
	glfwTerminate();
	return 0;
}