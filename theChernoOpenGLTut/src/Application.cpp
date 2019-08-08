#include <GL/glew.h>
#include <GLFW\glfw3.h>

#include <iostream>

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

	/* Format Data for use by OpenGL */
	unsigned int vertexBuffer;						//create ID tag
	glGenBuffers(1, &vertexBuffer);					//assign memory to tag
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	//switch OpenGL state to specified buffer

	/* Prepare Data before entering the loop */
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), imageData, GL_STATIC_DRAW);

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

	/* Shutdown GLFW and exit program */
	glfwTerminate();
	return 0;
}