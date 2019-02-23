
#include "RenderPass.hpp"

#include "math/math.hpp"

namespace RenderPass {

void DrawArrays(void* buffer, uint16_t num_vertices, const std::array<VertexAttribute, 16>& attributes, uint16_t num_attributes, uint16_t stride)
{
	// For now, let's assume we want to draw triangles
	uint16_t vertices_processed = 0;

	// This assumes that num_vertices is a multiple of 3! No easy way to check that in hardware.
	while (vertices_processed < num_vertices)
	{
		// Process 3 vertices into a triangle
		// Send the vertices and its data to a Raster Unit
		float ax, ay, az, bx, by, bz, cx, cy, cz;
		ax = ay = az = bx = by = bz = cx = cy = cz = 0;

		// For each attribute
		//for (int attr = 0; attr < num_attributes; ++attr)
		//{
		//	// Push the attribute value into an attribute buffer
		//}

		// Render the three vertices (the triangle)
		RenderTriangle(ax, ay, az, bx, by, bz, cx, cy, cz);

		vertices_processed += 3;
	}

}

void RenderTriangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
	// Apply vertex shader code on each vertex
	float x[3] = { ax, bx, cx };
	float y[3] = { ay, by, cy };
	float z[3] = { az, bz, cz };

	// Vertex shader : for each vertex
	//Mat4 matrix = Mat4::initIdentity() * Mat4::initCamera(Vec4(), Vec4(), Vec4()) * Mat4::initTranslation(Vec3(0,0,-20));
	for (int i = 0; i < 3; ++i)
	{
		// ----- VERTEX SHADER CODE ----- //
		Vec4 position(x[i], y[i], z[i], 1.0);
		//position = matrix * position;
		// ------------------------------ //

		// Division by w
		position.x /= position.w;
		position.y /= position.w;
		position.z /= position.w;
	}
	
	// Detect what type of triangle we are rendering
	bool firstType = true; // for now

	// Rasterize the triangle
	// TODO
}

}