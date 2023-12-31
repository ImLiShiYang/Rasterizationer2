﻿#ifndef RASTER_H
#define RASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>
#include <array>

#include "tgaimage.h"
#include "Triangle.h"
#include "OBJ_load.h"
#include "shader.h"



enum class TriangleVertexOrder
{
	counterclockwise,
	clockwise
};

class Rasterizer
{
public:
	Rasterizer(std::string file, TGAImage img);
	void draw_line(glm::vec3, glm::vec3);

	void draw(std::vector<std::shared_ptr<Mesh>> MeshList,IShader& shader);
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
	void SuthHodgClip(std::vector<Vertex>& poly_points, Triangle& triangle, glm::vec2 p1, glm::vec2 p2, std::vector<glm::vec4>& clipSpacePos);

	void MVP_Matrix();
	//void SetTheta(float t);

	void TurnOnBackCulling();
	void TurnOffBackCulling();
	void SetVertexOrder(const TriangleVertexOrder& t);
	TriangleVertexOrder GetVertexOrder();

	bool insideTriangle(const Triangle& m, const float x, const float y);
	void rasterize_edge_walking(const Triangle& m, const std::array<glm::vec4, 3>& clipSpacePos_Array);
	void rasterize_edge_equation(const Triangle& origin_m,const Triangle& m, std::vector<glm::vec4>& clipSpacePos, IShader& shader);

	Vertex MSAA(const Vertex& v);

private:
	TGAImage image;
	TGAImage color_buffer;
	const TGAColor white = TGAColor(255, 255, 255, 255);
	const TGAColor red = TGAColor(255, 0, 0, 255);
	std::string filename;
	int width, height;

	const int LEFT = 1;
	const int RIGHT = 2;
	const int BOTTOM = 4;
	const int TOP = 8;
	int xmin, xmax, ymin, ymax;

	//三角形顶点顺序
	TriangleVertexOrder vertexOrder = TriangleVertexOrder::counterclockwise;
	bool backCulling = false;

	std::vector<float> z_buffer;
	int get_index(int x, int y);
};


#endif // !RASTER_H



