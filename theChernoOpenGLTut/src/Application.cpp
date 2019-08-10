#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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

		VertexArray va;
		VertexBuffer vb(imageData, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(imageIndex, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 0.1f);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

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
			shader.Bind();
			shader.SetUniform4f("u_Color", r, g, b, 1.0f);

			va.Bind();
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
	}

	/* Shutdown GLFW and exit program */
	glfwTerminate();
	return 0;
}