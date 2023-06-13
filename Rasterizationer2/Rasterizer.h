#ifndef RASTER_H
#define RASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tgaimage.h"
#include "Triangle.h"

constexpr float MY_PI = 3.14159265359f;

class Rasterizer
{
public:
	Rasterizer(std::string file, TGAImage img);
	void draw_line(glm::vec3, glm::vec3);

	void draw();
	void output();
	void rasterize_wireframe(const Triangle& t);

	glm::mat4 Model_Matrix();
	glm::mat4 View_Matrix(glm::vec3 cameraPos, glm::vec3 center, glm::vec3 up);
	glm::mat4 Perspective_Matrix(float zneardis, float zfardis, float fovY, float aspect);
	glm::mat4 Orthographic_Matrix(float left, float bottom, float near, float right, float top, float far);
	glm::mat4 Viewport_Matrix(float width, float height);

	void MVP_Matrix();
	void SetTheta(float t);

private:
	TGAImage image;
	const TGAColor white = TGAColor(255, 255, 255, 255);
	const TGAColor red = TGAColor(255, 0, 0, 255);
	std::string filename;
	int width, height;

	glm::vec3 cameraPos;
	float theta;
};


#endif // !RASTER_H



