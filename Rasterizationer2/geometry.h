#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "tgaimage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

constexpr float MY_PI = 3.14159265359f;

class Vertex
{
public:
	Vertex() :vertex(), vertexColor(), normal(), texcoord(), worldPos(), cameraSpacePos(){}
	
	Vertex(glm::vec4 vertex):vertex(vertex), vertexColor(), normal(), texcoord(), worldPos() , cameraSpacePos(){}

	Vertex(glm::vec4 vertex, TGAColor vertexColor, glm::vec4 normal,glm::vec2 tex) :
		vertex(vertex), vertexColor(vertexColor), normal(normal),texcoord(tex), worldPos(), cameraSpacePos(){}

	Vertex(const glm::vec3& vertex,
		const TGAColor& vertexColor,
		const glm::vec3& normal,
		const glm::vec2& texcoord) :
		vertex(vertex.x, vertex.y, vertex.z, 1), vertexColor(vertexColor),
		normal(normal.x, normal.y, normal.z, 0), texcoord(texcoord) {}

	Vertex(glm::vec4 vertex, TGAColor vertexColor, glm::vec4 normal, glm::vec2 tex,glm::vec4 worldPos,glm::vec4 cameraSpacePos) :
		vertex(vertex), vertexColor(vertexColor), normal(normal), texcoord(tex), worldPos(worldPos), cameraSpacePos(cameraSpacePos){}

	Vertex operator+(const Vertex& v) const {
		return Vertex(vertex + v.vertex, vertexColor + v.vertexColor,
			normal + v.normal, texcoord + v.texcoord, worldPos + v.worldPos, cameraSpacePos+v.cameraSpacePos);
	}

	Vertex operator*(const float x) const {
		return Vertex(vertex * x, vertexColor * x, normal * x, texcoord * x, worldPos * x, cameraSpacePos * x);
	}
	Vertex& operator=(const Vertex& v) {
		vertex = v.vertex;
		vertexColor = v.vertexColor;
		normal = v.normal;
		texcoord = v.texcoord;
		worldPos = v.worldPos;
		cameraSpacePos = v.cameraSpacePos;
		return *this;
	}

public:
	glm::vec4 vertex;
	TGAColor vertexColor;
	glm::vec4 normal;
	glm::vec2 texcoord;
	glm::vec4 worldPos;
	glm::vec4 cameraSpacePos;
};

inline glm::vec4 lerp(glm::vec4 v1, glm::vec4 v2, float t)
{
	return v1 + (v2 - v1) * t;
}

inline glm::vec2 lerp(glm::vec2 v1, glm::vec2 v2, float t)
{
	return v1 + (v2 - v1) * t;
}


inline Vertex lerp(const Vertex& v1, const Vertex& v2, const float t) {
	return Vertex{
		lerp(v1.vertex, v2.vertex, t),
		ColorLerp(v1.vertexColor, v2.vertexColor, t),
		lerp(v1.normal, v2.normal, t),
		lerp(v1.texcoord, v2.texcoord, t),
		lerp(v1.worldPos, v2.worldPos, t),
		lerp(v1.cameraSpacePos, v2.cameraSpacePos, t)
	};
}

inline Vertex perspectiveLerp(const Vertex& v1, const Vertex& v2, const float t, const glm::vec4& v1c, const glm::vec4& v2c)
{
	float correctLerp = t * v2c.w / ((1 - t) * v1c.w + t * v2c.w);
	return lerp(v1, v2, correctLerp);
}

//透视矫正插值
inline float perspectiveLerp(const float t, const glm::vec4& v1c, const glm::vec4& v2c)
{
	float correctLerp = t * v2c.w / ((1 - t) * v1c.w + t * v2c.w);
	return correctLerp;
}

/*
inline Vertex barycentric_coordinates_perspective2(const Triangle& t, const glm::vec2& position_pixel, std::vector<glm::vec4>& v)
{
	Vertex v1 = t.vertex[0]; Vertex v2 = t.vertex[1]; Vertex v3 = t.vertex[2];
	float xa = v1.vertex.x; float ya = v1.vertex.y;
	float xb = v2.vertex.x; float yb = v2.vertex.y;
	float xc = v3.vertex.x; float yc = v3.vertex.y;
	float x = position_pixel.x; float y = position_pixel.y;

	float g = ((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) /
		((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
	float b = ((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) /
		((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
	float a = 1 - b - g;

	float W = 1.0f / (a / v[0].w + b / v[1].w + g / v[2].w);
	float beta = (b / v[1].w) * W;
	float gamma = (g / v[2].w) * W;
	float alpha = 1.0f - beta - gamma;

	return v1 * alpha + v2 * beta + v3 * gamma;
}
*/

inline Vertex barycentric_coordinates_perspective(const Vertex& pos, const Vertex& p0, const Vertex& p1, const Vertex& p2,
	std::vector<glm::vec4>& v)
{
	/*
	* 这里使用面积比的方式来求解重心坐标，
	* 给定两个三维向量A和B，它们的叉积为：
	* A x B = (Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx)
	* 叉积向量A x B垂直于向量A和B所在的平面，其方向遵循右手法则。
	* 叉积向量的长度等于向量A和B组成的平行四边形的面积：
	* |A x B| = |A| * |B| * sin(θ)
	* 其中|A|和|B|分别是向量A和B的长度，θ是它们之间的夹角。
	* 现在，考虑以二维向量表示的向量A和B。我们可以将这两个二维向量转换为三维向量，其中z分量为0，然后计算叉积。
	* 结果叉积向量的z分量表示两个二维向量所组成的平行四边形的有向面积。
	*/

	//判断二维三角形是否共线，也是使用向量叉积，只不过是二维，
	//所以Ay * Bz - Az * By, Az * Bx - Ax * Bz都等于0，就剩下Ax * By - Ay * Bx
	//然后化简得到公式代入计算
	/*
	glm::vec3 e0 = glm::vec3(p1.vertex.x - p0.vertex.x, p1.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 e1 = glm::vec3(p2.vertex.x - p1.vertex.x, p2.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 e2 = glm::vec3(p0.vertex.x - p2.vertex.x, p0.vertex.y - p2.vertex.y, 0.0f);

	glm::vec3 p0_diff = glm::vec3(pos.vertex.x - p0.vertex.x, pos.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 p1_diff = glm::vec3(pos.vertex.x - p1.vertex.x, pos.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 p2_diff = glm::vec3(pos.vertex.x - p2.vertex.x, pos.vertex.y - p2.vertex.y, 0.0f);

	float s0 = glm::cross(e0, p0_diff).z;
	float s1 = glm::cross(e1, p1_diff).z;
	float s2 = glm::cross(e2, p2_diff).z;

	//其中 u、v、w都>=0时，判断为三角形内，等于0在三角形边界上
	if (s0 >= 0 && s1 >= 0 && s2 >= 0)
	{
		float total_area = glm::cross(e0, -e2).z;
		glm::vec3 v(s1 / total_area, s2 / total_area, s0 / total_area);

		//在透视裁剪空间中，w=z
		float w = 1.0f / (v.x / clipSpacePos[0].w + v.y / clipSpacePos[1].w + v.z / clipSpacePos[2].w);

		float alpha = (v.x / clipSpacePos[0].w) * w;
		float belta = (v.y / clipSpacePos[1].w) * w;
		float gamma = (v.z / clipSpacePos[2].w) * w;

		return Vertex(p0 * alpha + p1 * belta + p2 * gamma);
	}
	return Vertex();
	*/

	float xa = p0.vertex.x; float ya = p0.vertex.y;
	float xb = p1.vertex.x; float yb = p1.vertex.y;
	float xc = p2.vertex.x; float yc = p2.vertex.y;
	float x = pos.vertex.x; float y = pos.vertex.y;

	float g = ((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) /
		((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
	float b = ((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) /
		((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
	float a = 1 - b - g;

	float W = 1.0f / (a / v[0].w + b / v[1].w + g / v[2].w);
	float beta = (b / v[1].w) * W;
	float gamma = (g / v[2].w) * W;
	float alpha = 1.0f - beta - gamma;

	Vertex ppp= p0 * alpha + p1 * beta + p2 * gamma;
	return ppp;
}

//法线变换矩阵
inline glm::vec4 GetNormal(glm::vec4 normal,glm::mat4 MV)
{
	MV = glm::transpose(glm::inverse(MV));
	return MV * normal;
}

// 计算重心坐标
inline Vertex barycentric_coordinates(const Vertex& pos, const Vertex& p0, const Vertex& p1, const Vertex& p2)
{
	/*
	* 这里使用面积比的方式来求解重心坐标，
	* 给定两个三维向量A和B，它们的叉积为：
	* A x B = (Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx)
	* 叉积向量A x B垂直于向量A和B所在的平面，其方向遵循右手法则。
	* 叉积向量的长度等于向量A和B组成的平行四边形的面积：
	* |A x B| = |A| * |B| * sin(θ)
	* 其中|A|和|B|分别是向量A和B的长度，θ是它们之间的夹角。
	* 现在，考虑以二维向量表示的向量A和B。我们可以将这两个二维向量转换为三维向量，其中z分量为0，然后计算叉积。
	* 结果叉积向量的z分量表示两个二维向量所组成的平行四边形的有向面积。
	*/

	//判断二维三角形是否共线，也是使用向量叉积，只不过是二维，
	//所以Ay * Bz - Az * By, Az * Bx - Ax * Bz都等于0，就剩下Ax * By - Ay * Bx
	//然后化简得到公式代入计算

	glm::vec3 e0 = glm::vec3(p1.vertex.x - p0.vertex.x, p1.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 e1 = glm::vec3(p2.vertex.x - p1.vertex.x, p2.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 e2 = glm::vec3(p0.vertex.x - p2.vertex.x, p0.vertex.y - p2.vertex.y, 0.0f);

	glm::vec3 p0_diff = glm::vec3(pos.vertex.x - p0.vertex.x, pos.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 p1_diff = glm::vec3(pos.vertex.x - p1.vertex.x, pos.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 p2_diff = glm::vec3(pos.vertex.x - p2.vertex.x, pos.vertex.y - p2.vertex.y, 0.0f);

	float s0 = glm::cross(e0, p0_diff).z;
	float s1 = glm::cross(e1, p1_diff).z;
	float s2 = glm::cross(e2, p2_diff).z;

	//其中 u、v、w都>=0时，判断为三角形内，等于0在三角形边界上
	if (s0 >= 0 && s1 >= 0 && s2 >= 0)
	{
		float total_area = glm::cross(e0, -e2).z;
		glm::vec3 v(s1 / total_area, s2 / total_area, s0 / total_area);
		return Vertex(p0 * v.x + p1 * v.y + p2 * v.z);
	}
	return Vertex();
}




// 返回两条线的交点的x值 
inline float x_intersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
{
	float num = (p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x);
	float den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	return num / den;
}

// 返回两条线的交点的y值
inline float y_intersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
{
	float num = (p1.x * p2.y - p1.y * p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x * p4.y - p3.y * p4.x);
	float den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	return num / den;
}

struct Line
{
	Vertex v1;
	Vertex v2;
	bool isNull = false;

	Line() :isNull(true) {}
	Line(const Vertex& v1, const Vertex& v2) :v1(v1), v2(v2) {}
};

inline glm::mat4 Model_Matrix(float theta, glm::vec3 rotateAxis)
{
	glm::mat4 matrix(1.0f);
	float angle = glm::radians(theta);
	matrix = glm::rotate(matrix, angle, rotateAxis);
	matrix = glm::translate(matrix, glm::vec3(0, -2, 0));
	return matrix;
}

inline glm::vec3 SetCamera(glm::vec3 camera, float theta, glm::vec3 rotateAxis)
{
	glm::mat4 matrix(1.0f);
	float angle = glm::radians(theta);
	matrix = glm::rotate(matrix, angle, rotateAxis);
	return matrix * glm::vec4(camera, 0);
}


inline glm::mat4 View_Matrix(glm::vec3 cameraPos, glm::vec3 center, glm::vec3 up)
{
	glm::vec3 z_vector = glm::normalize(cameraPos - center);
	glm::vec3 x_vector = glm::normalize(glm::cross(up, z_vector));
	glm::vec3 y_vector = glm::normalize(glm::cross(z_vector, x_vector));

	glm::mat4 R_view(1.0f), T_view(1.0f);
	T_view[3][0] = -cameraPos.x;
	T_view[3][1] = -cameraPos.y;
	T_view[3][2] = -cameraPos.z;

	R_view[0][0] = x_vector.x;
	R_view[1][0] = x_vector.y;
	R_view[2][0] = x_vector.z;

	R_view[0][1] = y_vector.x;
	R_view[1][1] = y_vector.y;
	R_view[2][1] = y_vector.z;

	R_view[0][2] = z_vector.x;
	R_view[1][2] = z_vector.y;
	R_view[2][2] = z_vector.z;

	return R_view * T_view;
}

inline glm::mat4 Orthographic_Matrix(float left, float bottom, float near, float right, float top, float far)
{
	glm::mat4 matrix(1.0f);
	matrix[0][0] = 2 / (right - left);
	matrix[1][1] = 2 / (top - bottom);
	matrix[2][2] = 2 / (near - far);

	matrix[3][0] = -(right + left) / (right - left);
	matrix[3][1] = -(top + bottom) / (top - bottom);
	matrix[3][2] = -(near + far) / (near - far);

	return matrix;
}

inline glm::mat4 Perspective_Matrix(float zneardis, float zfardis, float fovY, float aspect)
{
	float n = -zneardis;
	float f = -zfardis;
	float ffovY = fovY / 180.0 * MY_PI;

	glm::mat4 matrix(0.0f);
	matrix[0][0] = n;
	matrix[1][1] = n;
	matrix[2][2] = n + f;
	matrix[2][3] = 1;

	matrix[3][2] = -f * n;

	float t = std::tan(ffovY / 2) * abs(n);
	float b = -t;
	float r = aspect * t;
	float l = -r;

	return Orthographic_Matrix(l, b, n, r, t, f) * matrix;
}


inline glm::mat4 Viewport_Matrix(float width, float height)
{
	glm::mat4 matrix(1.0f);
	matrix[0][0] = width / 2;
	matrix[1][1] = height / 2;
	matrix[3][0] = width / 2;
	matrix[3][1] = height / 2;

	return matrix;
}


#endif // !GEOMETRY_H
