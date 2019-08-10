#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	/* Create in-stream object */
	std::ifstream stream(filepath);

	/* Specify Function Modes */
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
		std::cout << "failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	/* Return index for future use */
	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: Uniform '" << name << " does not exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}
