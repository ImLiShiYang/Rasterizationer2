#include "Triangle.h"
#include <iostream>


Triangle::Triangle()
{
}

Triangle::Triangle(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3)
{
	vertex[0].vertex = v1;
	vertex[1].vertex = v2;
	vertex[2].vertex = v3;

	//����������ʱ�ͶԶ�����ʱ������
	//EnsureCounterClockwiseOrder();
}


Triangle::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
	vertex[0] = v1;
	vertex[1] = v2;
	vertex[2] = v3;
}

Triangle::Triangle(Vertex v[3])
{
	vertex[0] = v[0];
	vertex[1] = v[1];
	vertex[2] = v[2];
}

Triangle::Triangle(glm::vec3 v[3])
{
	for (size_t i = 0; i < 3; i++)
	{
		setVertexPos(i, glm::vec4(v[i].x, v[i].y, v[i].z, 1));
	}
}

void Triangle::setVertexs(int index, const Vertex& v)
{
}


void Triangle::setNormal(int index, glm::vec4& newNorm)
{
	vertex[index].normal = newNorm;
}


void Triangle::setNormal(glm::vec4 newNorm[3])
{
	vertex[0].normal = newNorm[0];
	vertex[1].normal = newNorm[1];
	vertex[2].normal = newNorm[2];
}

void Triangle::setColor(int index, const TGAColor& newColor)
{
	vertex[index].vertexColor = newColor;
}

void Triangle::setVertexPos(int index, glm::vec4 newVert)
{
	vertex[index].vertex = newVert;
}

void Triangle::setColor(const TGAColor newColor[3])
{
	vertex[0].vertexColor = newColor[0];
	vertex[1].vertexColor = newColor[1];
	vertex[2].vertexColor = newColor[2];
}

void Triangle::setVertexPos(glm::vec4 newVert[3])
{
	vertex[0].vertex = newVert[0];
	vertex[1].vertex = newVert[1];
	vertex[2].vertex = newVert[2];
}



//����yֵ����ʹ��v1.y>v2.y>v3.y
/*
void Triangle::SortVertexsByY()
{
	if (vertex1.y < vertex2.y)
		std::swap(vertex1, vertex2);
	if (vertex1.y < vertex3.y)
		std::swap(vertex1, vertex3);
	if (vertex2.y < vertex3.y)
		std::swap(vertex2, vertex3);
}

bool Triangle::ThreePointCollinearity()
{
	glm::vec4 v1 = vertex2 - vertex1;
	glm::vec4 v2 = vertex3 - vertex1;
	glm::vec4 v = glm::cross(v1, v2);
	if (v == glm::vec4(0, 0, 0))
		return true;
	else
		return false;
}


void Triangle::EnsureCounterClockwiseOrder()
{
	//���������ֱ�ӷ���
	if (ThreePointCollinearity())
		return;

	// ���������ߣ�e1 �� e2��������
	glm::vec4 e1 = vertex2 - vertex1;
	glm::vec4 e2 = vertex3 - vertex1;

	// ���������������Ĳ�����õ�������
	glm::vec4 normal = glm::cross(e1, e2);

	// ������ߵ� Z ����С���㣬��ʾ������˳ʱ��˳����Ҫ������˳���Ϊ��ʱ�롣
	if (normal.z < 0)
	{
		std::swap(vertex2, vertex3);

		// ���� vertexs ����
		vertexs[0] = vertex1;
		vertexs[1] = vertex2;
		vertexs[2] = vertex3;
	}
}
*/
