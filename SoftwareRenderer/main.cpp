
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>

#include "Canvas.hpp"
#include "Rasterizer.hpp"
#include "MeshLoader.hpp"

// ----------------
// Globals
// ----------------

static GLFWwindow* window;

#define WIDTH 800
#define HEIGHT 800

// ----------------
// Functions
// ----------------

void Input();
void Update();
void Render();

void main(int argc, char* argv[])
{
	std::cout << "Software Renderer" << std::endl;

	// Create window
	if (!glfwInit())
	{
		assert(!"Failed to create a GLFW window!");
	}

	//glfwWindowHint;

	window = glfwCreateWindow(800, 800, "Software Renderer", nullptr, nullptr);

	// Make GL context current
	glfwMakeContextCurrent(window);

	// Vsync
	glfwSwapInterval(1);
	
	// Initialize Canvas
	Canvas::Init(WIDTH, HEIGHT);

	// Raster a triangle!
	Rasterizer::RasterizeTriangle(420, 500, 50, 250, 300, 255, 650, 300, 128, true);
	Rasterizer::RasterizeTriangle(400, 400, 50, 200, 200, 255, 600, 400, 255, false);
	Canvas::Update();

	// Load the 3D bunny
	int num_vertices = 0;
	std::vector<float> buffer = MeshLoader::LoadMesh("res/cube.obj", num_vertices);
	assert(num_vertices > 0);

	// Render the mesh

	// 60 FPS loop
	auto current_time = std::chrono::high_resolution_clock::now();
	auto start_time = std::chrono::high_resolution_clock::now();
	auto timer_time = std::chrono::high_resolution_clock::now();
	const double frame_time = (1.0 / 60.0) * 1000.0;
	unsigned int fps_count = 0, update_count = 0;
	std::string title = "Software Renderer";
	while (!glfwWindowShouldClose(window))
	{
		current_time = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() >= frame_time)
		{
			Input();
			Update();
			++update_count;
			Render();
			++fps_count;
			start_time = current_time;
		}

		if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - timer_time).count() >= 1000)
		{
			title = "Sotware Renderer     FPS = " + std::to_string(fps_count) + "  |  updates = " + std::to_string(update_count);
			glfwSetWindowTitle(window, title.c_str());
			fps_count = 0;
			update_count = 0;
			timer_time = current_time;
		}
	}
}

void Input()
{
	glfwPollEvents();
}

void Update()
{

}

void Render()
{
	Canvas::Render();

	glfwSwapBuffers(window);
}
