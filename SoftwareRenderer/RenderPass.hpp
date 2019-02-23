
#include <array>

typedef unsigned short uint16_t;

namespace RenderPass {

struct VertexAttribute 
{
	uint16_t index;
	uint16_t size; // in floats!
};

void DrawArrays(void* buffer, uint16_t num_vertices, const std::array<VertexAttribute, 16>& attributes, uint16_t num_attributes,  uint16_t stride);

void RenderTriangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz);

}