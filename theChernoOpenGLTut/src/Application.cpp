#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

	/* Handle Errors */
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

	unsigned int ibo;								//index ID
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	glBufferData									//Contextualize Index Data
	(
		GL_ELEMENT_ARRAY_BUFFER,	//Buffer Type
		6 * sizeof(unsigned int),	//Size of Buffer
		imageIndex,					//Data Location
		GL_STATIC_DRAW				//Draw Type
	);

	/* Load Data into OpenGL */
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), imageData, GL_STATIC_DRAW);

	/* Clear Bindings after use */
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Load or Create Shader Data */
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	/* Compile Shader */
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	
	/* Activate Shader */
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);				//reset framebuffer
		
		/* Render here */
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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