#include "Triangle.h"
#include <iostream>

Triangle::Triangle()
{
}

Triangle::Triangle(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3)
{
	vertex[0] = v1;
	vertex[1] = v2;
	vertex[2] = v3;

	//����������ʱ�ͶԶ�����ʱ������
	//EnsureCounterClockwiseOrder();
}



void Triangle::setVertexs(glm::vec4 newVert[3])
{
	vertex[0] = newVert[0];
	vertex[1] = newVert[1];
	vertex[2] = newVert[2];
}

void Triangle::setVertexs(int index, glm::vec4 newVert)
{
	vertex[index] = newVert;
}

void Triangle::setNormal(int index, glm::vec4& newNorm)
{
	normal[index] = newNorm;
}

void Triangle::setNormal(glm::vec4 newNorm[3])
{
	normal[0] = newNorm[0];
	normal[1] = newNorm[1];
	normal[2] = newNorm[2];
}

void Triangle::setColor(int index, const TGAColor& newColor)
{
	vertexColor[index] = newColor;
}

void Triangle::setColor(const TGAColor newColor[3])
{
	vertexColor[0] = newColor[0];
	vertexColor[1] = newColor[1];
	vertexColor[2] = newColor[2];
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
