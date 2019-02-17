
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>

#include "Canvas.hpp"

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

	window = glfwCreateWindow(800, 800, "Software Renderer", nullptr, nullptr);

	// Make GL context current
	glfwMakeContextCurrent(window);

	// Vsync
	glfwSwapInterval(1);
	
	// Initialize Canvas
	Canvas::Init(WIDTH, HEIGHT);

	for(int i = 0; i < 800; ++i)
		Canvas::Draw(i, 100, 0x00FFFF);
	Canvas::Update();

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