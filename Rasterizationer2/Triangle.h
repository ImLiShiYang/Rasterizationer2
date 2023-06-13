#ifndef TRIANGLE
#define TRIANGLE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tgaimage.h"

class Triangle
{
public:
	Triangle();
	Triangle(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3);

	void setVertexs(int index, glm::vec4 newVert);
	void setVertexs(glm::vec4 newVert[3]);

	void setNormal(int index, glm::vec4& newNorm);
	void setNormal(glm::vec4 newNorm[3]);
	void setColor(int index, const TGAColor& newColor);
	void setColor(const TGAColor newColor[3]);

	//根据y值排序，使得v1.y>v2.y>v3.y
	//void SortVertexsByY();

	//判断三角形的三个点是否共线,共线时返回true
	//bool ThreePointCollinearity();
	//对三角形顶点逆时针排序
	//void EnsureCounterClockwiseOrder();
	

public:
	glm::vec4 vertex[3];
	glm::vec4 normal[3];
	TGAColor vertexColor[3];
};

#endif // !TRIANGLE



