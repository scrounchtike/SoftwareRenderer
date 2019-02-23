
#include "Canvas.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <limits>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Canvas {

void Init(unsigned int sizex, unsigned int sizey)
{
	size_x = sizex;
	size_y = sizey;

	// Init glew
	if (glewInit() != GLEW_OK)
	{
		assert(!"Failed to initialize GLEW!");
	}

	// Init OpenGL
	glEnable(GL_TEXTURE_2D);

	glClearColor(0,0,0,0);

	// Load our 2D shader
	const char* vertex_shader_str = R"glsl(
		#version 410

		out vec2 texCoord0;

		uniform vec2 positions[6] = {
			vec2(-1,-1),
			vec2(-1,1),
			vec2(1,1),
			vec2(1,1),
			vec2(1,-1),
			vec2(-1,-1)
		};
		uniform vec2 texCoords[6] = {
			vec2(0,0),
			vec2(0,1),
			vec2(1,1),
			vec2(1,1),
			vec2(1,0),
			vec2(0,0)
		};

		void main(){
			gl_Position = vec4(positions[gl_VertexID], 0, 1);
			texCoord0 = texCoords[gl_VertexID];
		}
	)glsl";

	const char* fragment_shader_str = R"glsl(
		#version 410

		uniform sampler2D texSampler;

		in vec2 texCoord0;
		out vec4 FragColor;

		void main(){
			FragColor = texture(texSampler, texCoord0);
			//FragColor = vec4(1,0,1,1);
		}
	)glsl";

	const GLchar* vertex_shaders[1] = { vertex_shader_str };
	const GLchar* fragment_shaders[1] = { fragment_shader_str };

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shaders, nullptr);
	glShaderSource(fragment_shader, 1, fragment_shaders, nullptr);
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	// Get shader errors lambda
	auto GetError = [](bool shader_mode, GLuint input)
	{
		GLint max_length = 0;
		GLint length = 0;
		std::vector<GLchar> error_log;
		if (shader_mode)
		{
			glGetShaderiv(input, GL_INFO_LOG_LENGTH, &max_length);
			error_log.resize(max_length);
			glGetShaderInfoLog(input, max_length, &length, error_log.data());
		}
		else
		{
			glGetProgramiv(input, GL_INFO_LOG_LENGTH, &max_length);
			error_log.resize(max_length);
			glGetProgramInfoLog(input, max_length, &length, error_log.data());
		}
		for (int i = 0; i < length; ++i)
		{
			std::cout << error_log[i];
		}
	};

	GLint success = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	std::cout << success << std::endl;
	if (!success)
	{
		GetError(true, vertex_shader);
	}

	success = 0;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	std::cout << success << std::endl;
	if (!success)
	{
		GetError(true, fragment_shader);
	}

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	std::cout << success << std::endl;
	if (!success)
	{
		GetError(false, program);
	}

	int width, height, bpp;
	unsigned char* raw_data = stbi_load("test.png", &width, &height, &bpp, STBI_rgb);
	if (raw_data == nullptr) {
		std::cerr << "Error: Could not load PNG image in LoadTexturePNG : " << "test.png" << std::endl;
		return;
	}

	// Create 800x600 texture to render onto the screen
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	memset(screen, 0, sizeof(screen));

	glTexImage2D(GL_TEXTURE_2D, 0, 3, sizex, sizey, 0, GL_RGB, GL_UNSIGNED_BYTE, screen);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(raw_data);

	// Initialize depth buffer to be all INF
	float float_inf = std::numeric_limits<float>::infinity();
	for (int i = 0; i < 800; ++i)
	{
		for (int j = 0; j < 800; ++j)
		{
			depth_buffer[i][j] = float_inf;
		}
	}
}

void Clear(Color color)
{
	for (int i = 0; i < 800; ++i)
	{
		for (int j = 0; j < 600; ++j)
		{
			screen[i][j][0] = 255;
			screen[i][j][1] = 0;
			screen[i][j][2] = 255;
			screen[i][j][3] = 255;
		}
	}
}

void Draw(unsigned int x, unsigned int y, Color color)
{
	memcpy((void*)screen[y][x], &color, 3);
}

bool DrawIfNearer(unsigned int x, unsigned y, float z)
{
	// First, check the depth buffer to see current depth
	if (depth_buffer[x][y] < z)
		return false;
	// New value is nearer
	depth_buffer[x][y] = z;
	return true;
}

void DrawDepth(unsigned int x, unsigned int y, float z)
{
	depth_buffer[x][y] = z;
}

void Update()
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screen);
}

void Render()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

}