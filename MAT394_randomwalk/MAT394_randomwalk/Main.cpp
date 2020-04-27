
/* Start Header -------------------------------------------------------
File Name: Main.cpp
Purpose: Main for executing simulation
Language: C++
Platform: MSVC2019 window
Project: Random Walk Simulation
Author: Nahye Park
Creation date: 03/26/2020
End Header --------------------------------------------------------*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <time.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "Graph.hpp"
#include "Camera.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


int width = 1600, height = 1200;
float deltaTime, lastFrame = 0.f;
float aspects = float(width) / float(height);

struct GuiVar {
	int MAX_STEP = 1000;
	bool autoplay = false;
	int xmin, xmax, ymin, ymax, zmin, zmax;
	bool display_loop = false;
	int xSize = 100;
	int ySize = 100;
	int zSize = 100;
};

struct Result {
	int steps;
	float ave_dist;
	float ave_largest;
	float ave_num_loop;
};


glm::vec3 colors[9] = {
	{ 1.0f, 0.f, 0.f }, //red
	{ 1.0f, 0.5f, 0.f }, //orange
	{ 1.0f, 1.f, 0.f }, //yellow
	{ 0.5f, 1.f, 0.0f }, //lightgreen
	{ 0.0f, 1.f, 0.f }, //green
	{ 0.0f, 1.f, 1.f }, //lightblue
	{ 0.0f, 0.f, 1.f }, //blue
	{ 0.9f, 0.5f, 0.9f }, //violet
	{ 0.86f, 0.6f, 0.86f } // plum
};

bool simulation_start = false;
bool prob_simulation = false;
std::vector<std::pair<int, float>> prob_result;

// Set camera's position
Camera camera(glm::vec3(50.f,10.f, 25.f));

int main(void) {
	
	GuiVar manage;
	std::vector<Result> result;

	srand(time(NULL));
	camera.aspect = aspects;
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Random Walk", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark background
	glClearColor(0.f, 0.f, 0.f, 0.0f);

	//IMGUI
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("VertexShader.vert", "FragmentShader.frag");

	//COORDINATE
	std::vector<Line> coord;
	coord.push_back(Line(100.f, LineType::X, glm::vec3(1.f, 1.f, 1.f)));
	coord.push_back(Line(100.f, LineType::Y, glm::vec3(1.f, 1.f, 1.f)));
	coord.push_back(Line(100.f, LineType::Z, glm::vec3(1.f, 1.f, 1.f)));

	//GRAPH LINE
	Line line;
	Line loop_line;
	loop_line.color = glm::vec3(1.f, 1.f, 1.f);

	//HEAD SPHERE
	Sphere head;
	//RANDOM WALK STUFF
	RandomWalk rw;

	//simulation & analysis
	bool simulation = true;
	int numerical_steps = 10;
	int prob_steps = 100;


	float deltaTime = 0;
	do {

		ImGui_ImplGlfwGL3_NewFrame();

		float currFrame = (float)glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);


		//////////////////IMGUI////////////////////////////////////
		{
			ImGui::Begin("Simulation");
			if (ImGui::Button("Visual Simulation"))
			{
				simulation = true;
				numerical_steps = 10;
				result.clear();
				simulation_start = false;
			}
			if (ImGui::Button("Numerical Simulation"))
				simulation = false;


			ImGui::End();


			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::NewLine();
			if (ImGui::Button("Normal Random Walk"))
			{
				if (rw.looperased)
					rw.Reset();
				rw.looperased = false;
				rw.loop_exist = false;
				prob_simulation = false;
			}ImGui::SameLine();
			if (ImGui::Button("Loop Erased Random Walk"))
			{
				if (!rw.looperased)
					rw.Reset();
				rw.looperased = true;
				prob_simulation = false;
			}
			if (!simulation)
			{
				if (ImGui::Button("Probability to Return to Origin"))
				{
					prob_result.clear();
					prob_steps = 100;
					simulation_start = true;
					prob_simulation = true;
				}
			}
			
			ImGui::NewLine();

			if (rw.looperased)
				ImGui::Text("Current Mode : Loop Erased Random Walk");
			else
				ImGui::Text("Current Mode : Normal Random Walk in 3D");
			ImGui::Text("Total steps : %i", rw.steps);
			if(rw.loop_exist)
				ImGui::Text("Size of steps : %i", (int)rw.points.size()-2+rw.size_loop);
			else
				ImGui::Text("Size of steps : %i", (int)rw.points.size() - 2);
			ImGui::Text("Current Position : (%i , %i, %i)", (int)rw.points.back().x, (int)rw.points.back().y, (int)rw.points.back().z);
			ImGui::Text("Distance from Origin : %.3f", rw.Distance());
			ImGui::NewLine();
			
			if(simulation)
				ImGui::SliderInt("Max Step",&manage.MAX_STEP, 100, 10000);
			ImGui::NewLine();
			ImGui::Checkbox("Set Limit", &rw.limit);
			ImGui::NewLine();
			if (rw.limit)
			{
				ImGui::Text("Limit of X : %i to %i", (int)rw.limit_min.x, (int)rw.limit_max.x);
				ImGui::Text("Limit of Y : %i to %i", (int)rw.limit_min.y, (int)rw.limit_max.y);
				ImGui::Text("Limit of Z : %i to %i", (int)rw.limit_min.z, (int)rw.limit_max.z);
				ImGui::NewLine();
				ImGui::SliderInt("Size of X", &manage.xSize, 10, 200);
				ImGui::SliderInt("Size of Y", &manage.ySize, 10, 200);
				ImGui::SliderInt("Size of Z", &manage.zSize, 10, 200);

				rw.limit_max.x = manage.xSize * 0.5f;
				rw.limit_max.y = manage.ySize * 0.5f;
				rw.limit_max.z = manage.zSize * 0.5f;
				rw.limit_min.x = -manage.xSize * 0.5f;
				rw.limit_min.y = -manage.ySize * 0.5f;
				rw.limit_min.z = -manage.zSize * 0.5f;

			}ImGui::NewLine();

			if (simulation)
			{
				if (ImGui::Button("Restart"))
				{
					rw.Reset();
				}ImGui::NewLine();


				if (ImGui::Button("Auto Play"))
				{
					manage.autoplay = true;
				}ImGui::SameLine();
				if (ImGui::Button("Play Step by Step"))
				{
					manage.autoplay = false;
				}
				if (rw.looperased)
				{
					ImGui::Begin("Loop Erased Random Walk");
					ImGui::Text("the number of erased loop : %i", rw.num_loop);
					ImGui::NewLine();
					ImGui::Text("Largest loop size : %i", rw.biggest_loop);

					if (rw.loop_exist)
					{
						ImGui::Text("Current loop size : %i", rw.size_loop);
						if (manage.autoplay)
							rw.loop_exist = false;
					}



					ImGui::End();


				}


				if (!manage.autoplay)
				{
					ImGui::Begin("Control steps");
					if (ImGui::Button("Next Step"))
					{
						if (rw.looperased)
						{
							if (!rw.loop_exist)
								rw.Walk();
							else if (rw.loop_exist)
							{
								rw.loop_exist = false;
								++rw.num_loop;
							}

						}
						else
							rw.Walk();

					}ImGui::SameLine();
					if (!rw.looperased)
					{
						if (ImGui::Button("Remove Last Step"))
						{
							rw.RemoveLast();
						}
					}
					ImGui::End();

				}

			}
			else
			{

				if (ImGui::Button("Start"))
				{
					result.clear();
					numerical_steps = 10;
					simulation_start = true;
				}
				if (simulation_start)
				{
					ImGui::SameLine();
					ImGui::Text(" Now Calculating ...");
				}

				ImGui::Begin("Result");
				if(prob_simulation)
					ImGui::Text("	STEPS			Probability to Return to Origin");
				else
				{
					if (!rw.looperased)
						ImGui::Text("	  STEPS		  average distance");
					else
						ImGui::Text("	  STEPS		  average distance		average largest loop		average erased loop");
				}

				if (prob_simulation)
				{
					for(int k = 0; k < prob_result.size(); ++k)
						ImGui::Text("%8i steps			%.6f" , prob_result[k].first, prob_result[k].second);
				}
				else
				{
					for (int k = 0; k < result.size(); ++k)
					{


						if (!rw.looperased)
							ImGui::Text("%5i steps			%3.3f", result[k].steps, result[k].ave_dist);
						else
							ImGui::Text("%5i steps			%3.3f					%.3f						%.5f", result[k].steps, result[k].ave_dist, result[k].ave_largest, result[k].ave_num_loop);


					}

				}
				ImGui::End();

			}
			
			
		}
		////////////////////////////////////////////////////////////////////////////////////


		ProcessInput(&camera, window, deltaTime);

		if (simulation)
		{

			if (rw.looperased && !rw.loop_exist)
				rw.CheckLoop();


			if (rw.steps < manage.MAX_STEP && manage.autoplay && !rw.loop_exist)
			{
				rw.Walk();
			}

		
			//DRAW COORDINATE WORLD
			for (unsigned i = 0; i < coord.size(); ++i)
			{
				coord[i].m_Projection = camera.GetProjectionMatirx();
				coord[i].m_View = camera.GetViewMatrix();
				coord[i].Draw(programID, camera.position);
			}

			line.m_Projection = camera.GetProjectionMatirx();
			line.m_View = camera.GetViewMatrix();

			loop_line.m_Projection = camera.GetProjectionMatirx();
			loop_line.m_View = camera.GetViewMatrix();

			head.m_Projection = camera.GetProjectionMatirx();
			head.m_View = camera.GetViewMatrix();
			head.center = rw.points.back();
			head.scale = glm::vec3(0.3f, 0.3f, 0.3f);
			head.Draw(programID, camera.position);

			int colorindex = -1;
			//DRAW LINES
			for (size_t i = 0; i < rw.points.size() - 1; ++i)
			{
				//set color
				if (i % 40 == 0)
				{
					++colorindex;
					if (colorindex > 8)
						colorindex = 0;
				}

				line.color = colors[colorindex];
				line.SetStartEnd(rw.points[i], rw.points[i + 1]);
				line.Draw(programID, camera.position);
			}


			if (rw.loop_exist)
			{
				for (size_t i = 0; i < rw.loop.size() - 1; ++i)
				{
					loop_line.SetStartEnd(rw.loop[i], rw.loop[i + 1]);
					loop_line.Draw(programID, camera.position);
				}

				if (manage.autoplay)
					++rw.num_loop;

			}

		}
		if(simulation_start && !prob_simulation)
		{
			
			Result l_result;

			l_result.steps = numerical_steps;
			if (rw.looperased)
				LoopErasedSimulation(l_result.steps, rw, l_result.ave_dist, l_result.ave_largest, l_result.ave_num_loop);
			else
				NormalSimulation(l_result.steps, rw, l_result.ave_dist);

			result.push_back(l_result);

			numerical_steps *= 10;
			if (numerical_steps == 1000000)
				simulation_start = false;
		}
		if (prob_simulation && simulation_start)
		{
			float l_prob;

			ProbabilityToReturn(rw, l_prob, prob_steps);
			prob_result.push_back(std::make_pair(prob_steps, l_prob));

			prob_steps *= 10;
			if (prob_steps == 1000000)
				simulation_start = false;
			

		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	coord[0].Clear();
	coord[1].Clear();
	coord[2].Clear();

	line.Clear();

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
	
}