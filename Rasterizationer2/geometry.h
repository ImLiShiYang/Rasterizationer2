#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "tgaimage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex
{
public:
	Vertex() :vertex(), vertexColor(), normal() {}
		
	Vertex(glm::vec4 vertex, TGAColor vertexColor, glm::vec4 normal) :
		vertex(vertex), vertexColor(vertexColor), normal(normal) {}

	Vertex operator+(const Vertex& v) const {
		return Vertex(vertex + v.vertex, vertexColor + v.vertexColor, normal + v.normal);
	}
	Vertex operator*(const float x) const {
		return Vertex(vertex * x, vertexColor * x, normal * x);
	}
	Vertex& operator=(const Vertex& v) {
		vertex = v.vertex;
		vertexColor = v.vertexColor;
		normal = v.normal;
		return *this;
	}

public:
	glm::vec4 vertex;
	TGAColor vertexColor;
	glm::vec4 normal;
};

inline glm::vec4 lerp(glm::vec4 v1, glm::vec4 v2, float t)
{
	return v1 + (v2 - v1) * t;
}

inline Vertex lerp(const Vertex& v1, const Vertex& v2, const float t) {
	return Vertex{
		lerp(v1.vertex, v2.vertex, t),
		ColorLerp(v1.vertexColor, v2.vertexColor, t),
		lerp(v1.normal, v2.normal, t)
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


#endif // !GEOMETRY_H
