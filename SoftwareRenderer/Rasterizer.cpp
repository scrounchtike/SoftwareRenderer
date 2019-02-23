
#include "rasterizer.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "Canvas.hpp"

namespace Rasterizer {

// Assuming its vertices are in 2 y-levels. (A is highest or B is lowest)
// Counterclockwise ordering
// And that b.x < c.x and that a.y > b.y
void RasterizeTriangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz, bool first_type)
{
	// Current position of pixel
	int pixel_x_min = 0;
	int pixel_x_max = 0;
	int pixel_y = 0;

	// First line to calculate
	float delta_y = ay - by; // This should always be > 0
	assert(delta_y > 0);
	float delta_x_min = bx - ax;
	float delta_x_max;
	float delta_z_min = bz - az;
	float delta_z_max;
	if (first_type)
	{
		delta_x_max = cx - ax;
		delta_z_max = cz - az;
	}
	else
	{
		delta_x_max = bx - cx;
		delta_z_max = bz - cz;
	}

	// TODO: Dont do divisions, use bresenham!!
	float step_x_min = delta_x_min / delta_y;
	float step_x_max = delta_x_max / delta_y;
	float step_z_min = delta_z_min / delta_y;
	float step_z_max = delta_z_max / delta_y;
	float step_y = -1;

	// We start at the highest yline
	int ayint = std::floor(ay);
	pixel_y = ayint;

	// For the first step, do special calc to find the x and z that intersects the yline
	float a_y_floatcomp = ay - ayint;
	float first_step_x_min = step_x_min * a_y_floatcomp;
	float first_step_x_max = step_x_max * a_y_floatcomp;
	float first_step_z_min = step_z_min * a_y_floatcomp;
	float first_step_z_max = step_z_max * a_y_floatcomp;

	float xlocation_min = ax + first_step_x_min;
	float xlocation_max;
	float zlocation_min = az + first_step_z_min;
	float zlocation_max;
	if (first_type)
	{
		xlocation_max = ax + first_step_x_max;
		zlocation_max = az + first_step_z_max;
	}
	else 
	{
		xlocation_max = cx + first_step_x_max;
		zlocation_max = cz + first_step_z_max;
	}

	pixel_x_min = std::floor(xlocation_min);
	pixel_x_max = std::floor(xlocation_max);

	// Render the first point(s)
	float zlocation = zlocation_min;
	float delta_x_yline = pixel_x_max - pixel_x_min;
	float delta_z_yline = zlocation_max - zlocation_min;
	float step_z = delta_z_yline / delta_x_yline;
	for (int x = pixel_x_min; x <= pixel_x_max; ++x)
	{
		zlocation += step_z;

		if(Canvas::DrawIfNearer(x, pixel_y, zlocation))
			Canvas::Draw(x, pixel_y, (int)std::floor(zlocation) << (first_type ? 8 : 0));
	}

	// Iterate to find all the remaining pixels
	while (true)
	{
		// Decrement our ylevel by one
		pixel_y += step_y;

		// ---- MIN ---- //
		xlocation_min += step_x_min;
		pixel_x_min = std::floor(xlocation_min);
		zlocation_min += step_z_min;

		// ---- MAX ---- //
		xlocation_max += step_x_max;
		pixel_x_max = std::floor(xlocation_max);
		zlocation_max += step_z_max;

		// We now have the min and max pixels for the yline of the triangle
		// Render the pixels
		zlocation = zlocation_min;
		float delta_x_yline = pixel_x_max - pixel_x_min;
		float delta_z_yline = zlocation_max - zlocation_min;
		float step_z = delta_z_yline / delta_x_yline;
		for (int x = pixel_x_min; x <= pixel_x_max; ++x)
		{
			// Increment the z position by step_z
			zlocation += step_z;

			if (Canvas::DrawIfNearer(x, pixel_y, zlocation))
				Canvas::Draw(x, pixel_y, (int)std::floor(zlocation) << (first_type ? 8 : 0));
		}

		// Check if we have passed our target
		if (pixel_y <= by) break;
	}
}

}