#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

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
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
		{
			-100.0f, -100.0f, 0.0f, 0.0f,
			 100.0f, -100.0f, 1.0f, 0.0f,
			 100.0f,  100.0f, 1.0f, 1.0f,
			-100.0f,  100.0f, 0.0f, 1.0f,
		};

		unsigned int imageIndex[] =
		{
			0, 1, 2,		// triangle 1
			2, 3, 0,		// triangle 2
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(imageData, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(imageIndex, 6);

		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 model;
		glm::mat4 mvp;

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Texture texture("res/textures/Sigil.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);


		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		float color[3] = { 0 };
		float increment[3] = { 0.09f, 0.06f, 0.03f };

		glm::vec3 pos1(0.0f);
		glm::vec3 pos2(0.0f);
		
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();

			shader.Bind();
			{
				model = glm::translate(glm::mat4(1.0f), pos1);
				mvp = proj * view * model;

				shader.SetUniformMat4f("u_MVP", mvp);
				shader.SetUniform4f("u_Color", color[0], color[1], color[2], 1.0f);

				renderer.Draw(va, ib, shader);
			}

			shader.Bind();
			{
				model = glm::translate(glm::mat4(1.0f), pos2);
				mvp = proj * view * model;

				shader.SetUniformMat4f("u_MVP", mvp);
				shader.SetUniform4f("u_Color", color[0], color[1], color[2], 1.0f);

				renderer.Draw(va, ib, shader);
			}

			for (int i = 0; i < 3; i++)
			{
				Bounce(color[i], increment[i]);
				color[i] += increment[i];
			}

			ImGui::SliderFloat2("Translation A", &pos1.x, 0.0f, 1000.0f);
			ImGui::SliderFloat2("Translation B", &pos2.x, 0.0f, 1000.0f);

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	/* Shutdown GLFW and exit program */
	glfwTerminate();
	return 0;
}