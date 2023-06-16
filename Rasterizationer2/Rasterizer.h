#ifndef RASTER_H
#define RASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

#include "tgaimage.h"
#include "Triangle.h"

constexpr float MY_PI = 3.14159265359f;

class Rasterizer
{
public:
	Rasterizer(std::string file, TGAImage img);
	void draw_line(glm::vec3, glm::vec3);

	void draw(std::vector<std::shared_ptr<Triangle>>& TriangleList);
	void output();
	void rasterize_wireframe(const Triangle& t);

	//屏幕空间裁剪算法
	//编码裁剪算法
	std::vector<Line> clip_Cohen_Sutherland(const Triangle& t, std::vector<glm::vec4> clipSpacePos);
	int GetCode(float x, float y);
	void CodeClip(Line& line, std::vector<glm::vec4> clipSpacePos);

	//Sutherland-Hodgman多边形裁切算法
	std::vector<Triangle> SuthHodgClipTriangle(Triangle& triangle, std::vector<glm::vec4>& clipSpacePos);
	//此函数根据裁剪区域的一个裁剪边,裁剪所有边缘
	void SuthHodgClip(std::vector<Vertex>& poly_points, glm::vec2 p1, glm::vec2 p2, std::vector<glm::vec4>& clipSpacePos);

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

	float zneardis;
	float zfardis;
	float fovY;
	float aspect;

	glm::vec3 cameraPos;
	float theta;

	const int LEFT = 1;
	const int RIGHT = 2;
	const int BOTTOM = 4;
	const int TOP = 8;
	int xmin, xmax, ymin, ymax;
};


#endif // !RASTER_H



