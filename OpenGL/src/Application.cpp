#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


int main()
{
#pragma region Init

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error !" << std::endl;
	/*
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl;
	*/
#pragma endregion

	{
		float positions[] = {
			-545.f, -411.f, 0.0f, 0.0f,
			 545.f, -411.f, 1.0f, 0.0f,
			 545.f,  411.f, 1.0f, 1.0f,
			-545.f,  411.f, 0.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(-640.0f, 640.0f, -360.f, 360.f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(640, 0, 0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 360, 0));

		glm::mat4 mvp = proj * view * model;

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.1f, 0.2f, 0.4f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		Texture texture("res/textures/Nioh_Logo.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		Renderer renderer;

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
		

		float b = 0.0f;
		// In order to use vec2 for the sliders to have them on the same line and less code redundancy,
		// BUT to also have coherent min and max values, I use a vec2 per axis instead of vec2 per image
		glm::vec2 horizontal(0, 0), vertical(0, 0);
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			shader.SetUniform4f("u_Color", 0.1f, 0.2f, b, 1.0f);
			{
				model = glm::translate(glm::mat4(1.0f), glm::vec3(horizontal.x, vertical.x, 0));
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				
				renderer.Draw(va, ib, shader);
			}

			{
				model = glm::translate(glm::mat4(1.0f), glm::vec3(horizontal.y, vertical.y, 0));
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				
				renderer.Draw(va, ib, shader);
			}

			if (b >= 1.0f) increment = -0.05f;
			else if (b <= 0.0f) increment = 0.05f;
			b += increment;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				ImGui::Begin("Move ");
				ImGui::SliderFloat2("Horizontal translations", &horizontal.x, -1920.0f, 640.0f);
				ImGui::SliderFloat2("Vertical translations", &vertical.x, -720.f, 720.f);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	glfwTerminate();
	return 0;
}
