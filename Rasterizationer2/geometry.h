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
	Vertex() :vertex(), vertexColor(), normal(), texcoord(), worldPos() {}
		
	Vertex(glm::vec4 vertex):vertex(vertex), vertexColor(), normal(), texcoord(), worldPos() {}

	Vertex(glm::vec4 vertex, TGAColor vertexColor, glm::vec4 normal,glm::vec2 tex) :
		vertex(vertex), vertexColor(vertexColor), normal(normal),texcoord(tex), worldPos(){}

	Vertex(const glm::vec3& vertex,
		const TGAColor& vertexColor,
		const glm::vec3& normal,
		const glm::vec2& texcoord) :
		vertex(vertex.x, vertex.y, vertex.z, 1), vertexColor(vertexColor),
		normal(normal.x, normal.y, normal.z, 0), texcoord(texcoord) {}

	Vertex(glm::vec4 vertex, TGAColor vertexColor, glm::vec4 normal, glm::vec2 tex,glm::vec4 worldPos) :
		vertex(vertex), vertexColor(vertexColor), normal(normal), texcoord(tex), worldPos(worldPos){}

	Vertex operator+(const Vertex& v) const {
		return Vertex(vertex + v.vertex, vertexColor + v.vertexColor,
			normal + v.normal, texcoord + v.texcoord, worldPos + v.worldPos);
	}

	Vertex operator*(const float x) const {
		return Vertex(vertex * x, vertexColor * x, normal * x, texcoord * x, worldPos * x);
	}
	Vertex& operator=(const Vertex& v) {
		vertex = v.vertex;
		vertexColor = v.vertexColor;
		normal = v.normal;
		texcoord = v.texcoord;
		worldPos = v.worldPos;
		return *this;
	}

public:
	glm::vec4 vertex;
	TGAColor vertexColor;
	glm::vec4 normal;
	glm::vec2 texcoord;
	glm::vec4 worldPos;
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
		lerp(v1.worldPos, v2.worldPos, t)
	};
}

inline Vertex perspectiveLerp(const Vertex& v1, const Vertex& v2, const float t, const glm::vec4& v1c, const glm::vec4& v2c)
{
	float correctLerp = t * v2c.w / ((1 - t) * v1c.w + t * v2c.w);
	return lerp(v1, v2, correctLerp);
}

//͸�ӽ�����ֵ
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

	return p0 * alpha + p1 * beta + p2 * gamma;

}

//���߱任����
inline glm::vec4 GetNormal(glm::vec4 normal,glm::mat4 MV)
{
	MV = glm::transpose(glm::inverse(MV));
	return MV * normal;
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
