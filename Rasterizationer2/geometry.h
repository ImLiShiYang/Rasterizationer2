#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "tgaimage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

class Vertex
{
public:
	Vertex() :vertex(), vertexColor(), normal() {}
		
	Vertex(glm::vec4 vertex):vertex(vertex), vertexColor(), normal() {}

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

inline Vertex barycentric_coordinates_perspective(const Vertex& pos, const Vertex& p0, const Vertex& p1, const Vertex& p2,
	std::vector<glm::vec4>& clipSpacePos)
{
	/*
	* ����ʹ������ȵķ�ʽ������������꣬
	* ����������ά����A��B�����ǵĲ��Ϊ��
	* A x B = (Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx)
	* �������A x B��ֱ������A��B���ڵ�ƽ�棬�䷽����ѭ���ַ���
	* ��������ĳ��ȵ�������A��B��ɵ�ƽ���ı��ε������
	* |A x B| = |A| * |B| * sin(��)
	* ����|A|��|B|�ֱ�������A��B�ĳ��ȣ���������֮��ļнǡ�
	* ���ڣ������Զ�ά������ʾ������A��B�����ǿ��Խ���������ά����ת��Ϊ��ά����������z����Ϊ0��Ȼ���������
	* ������������z������ʾ������ά��������ɵ�ƽ���ı��ε����������
	*/

	//�ж϶�ά�������Ƿ��ߣ�Ҳ��ʹ�����������ֻ�����Ƕ�ά��
	//����Ay * Bz - Az * By, Az * Bx - Ax * Bz������0����ʣ��Ax * By - Ay * Bx
	//Ȼ�󻯼�õ���ʽ�������

	glm::vec3 e0 = glm::vec3(p1.vertex.x - p0.vertex.x, p1.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 e1 = glm::vec3(p2.vertex.x - p1.vertex.x, p2.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 e2 = glm::vec3(p0.vertex.x - p2.vertex.x, p0.vertex.y - p2.vertex.y, 0.0f);

	glm::vec3 p0_diff = glm::vec3(pos.vertex.x - p0.vertex.x, pos.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 p1_diff = glm::vec3(pos.vertex.x - p1.vertex.x, pos.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 p2_diff = glm::vec3(pos.vertex.x - p2.vertex.x, pos.vertex.y - p2.vertex.y, 0.0f);

	float s0 = glm::cross(e0, p0_diff).z;
	float s1 = glm::cross(e1, p1_diff).z;
	float s2 = glm::cross(e2, p2_diff).z;

	//���� u��v��w��>=0ʱ���ж�Ϊ�������ڣ�����0�������α߽���
	if (s0 >= 0 && s1 >= 0 && s2 >= 0)
	{
		float total_area = glm::cross(e0, -e2).z;
		glm::vec3 v(s1 / total_area, s2 / total_area, s0 / total_area);

		//��͸�Ӳü��ռ��У�w=z
		float w = 1.0f / (v.x / clipSpacePos[0].w + v.y / clipSpacePos[1].w + v.z / clipSpacePos[2].w);

		float alpha = (v.x / clipSpacePos[0].w) * w;
		float belta = (v.y / clipSpacePos[1].w) * w;
		float gamma = (v.z / clipSpacePos[2].w) * w;

		return Vertex(p0 * alpha + p1 * belta + p2 * gamma);
	}
	return Vertex();
}


// ������������
inline Vertex barycentric_coordinates(const Vertex& pos, const Vertex& p0, const Vertex& p1, const Vertex& p2)
{
	/*
	* ����ʹ������ȵķ�ʽ������������꣬
	* ����������ά����A��B�����ǵĲ��Ϊ��
	* A x B = (Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx)
	* �������A x B��ֱ������A��B���ڵ�ƽ�棬�䷽����ѭ���ַ���
	* ��������ĳ��ȵ�������A��B��ɵ�ƽ���ı��ε������
	* |A x B| = |A| * |B| * sin(��)
	* ����|A|��|B|�ֱ�������A��B�ĳ��ȣ���������֮��ļнǡ�
	* ���ڣ������Զ�ά������ʾ������A��B�����ǿ��Խ���������ά����ת��Ϊ��ά����������z����Ϊ0��Ȼ���������
	* ������������z������ʾ������ά��������ɵ�ƽ���ı��ε����������
	*/

	//�ж϶�ά�������Ƿ��ߣ�Ҳ��ʹ�����������ֻ�����Ƕ�ά��
	//����Ay * Bz - Az * By, Az * Bx - Ax * Bz������0����ʣ��Ax * By - Ay * Bx
	//Ȼ�󻯼�õ���ʽ�������

	glm::vec3 e0 = glm::vec3(p1.vertex.x - p0.vertex.x, p1.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 e1 = glm::vec3(p2.vertex.x - p1.vertex.x, p2.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 e2 = glm::vec3(p0.vertex.x - p2.vertex.x, p0.vertex.y - p2.vertex.y, 0.0f);

	glm::vec3 p0_diff = glm::vec3(pos.vertex.x - p0.vertex.x, pos.vertex.y - p0.vertex.y, 0.0f);
	glm::vec3 p1_diff = glm::vec3(pos.vertex.x - p1.vertex.x, pos.vertex.y - p1.vertex.y, 0.0f);
	glm::vec3 p2_diff = glm::vec3(pos.vertex.x - p2.vertex.x, pos.vertex.y - p2.vertex.y, 0.0f);

	float s0 = glm::cross(e0, p0_diff).z;
	float s1 = glm::cross(e1, p1_diff).z;
	float s2 = glm::cross(e2, p2_diff).z;

	//���� u��v��w��>=0ʱ���ж�Ϊ�������ڣ�����0�������α߽���
	if (s0 >= 0 && s1 >= 0 && s2 >= 0)
	{
		float total_area = glm::cross(e0, -e2).z;
		glm::vec3 v(s1 / total_area, s2 / total_area, s0 / total_area);
		return Vertex(p0 * v.x + p1 * v.y + p2 * v.z);
	}
	return Vertex();
}



//͸�ӽ�����ֵ
inline float perspectiveLerp(const float t, const glm::vec4& v1c, const glm::vec4& v2c)
{
	float correctLerp = t * v2c.w / ((1 - t) * v1c.w + t * v2c.w);
	return correctLerp;
}

// ���������ߵĽ����xֵ 
inline float x_intersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
{
	float num = (p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x);
	float den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	return num / den;
}

// ���������ߵĽ����yֵ
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
