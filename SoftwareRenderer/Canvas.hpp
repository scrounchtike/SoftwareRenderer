
#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef int Color; // RGB

namespace Canvas {

	static unsigned int size_x;
	static unsigned int size_y;

	static GLuint program;
	static GLuint texture;

	//static unsigned char screen[800][600][4];
	static unsigned char screen[800][800][3];

	void Init(unsigned int sizex, unsigned int sizey);
	void Clear(Color color);
	void Draw(unsigned int x, unsigned int y, Color color);
	void Update();
	void Render();
}

#endif