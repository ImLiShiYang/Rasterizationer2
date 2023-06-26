#ifndef TRIANGLE
#define TRIANGLE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

#include "tgaimage.h"
#include "geometry.h"

class Triangle
{
public:
	Triangle();
	Triangle(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3);
	Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
	Triangle(glm::vec3 v[3]);
	Triangle(Vertex v[3]);

	void setVertexs(int index, const Vertex& v);
	void setVertexs(const Vertex v[3]);

	void setVertexPos(int index, glm::vec4 newVert);
	void setVertexPos(glm::vec4 newVert[3]);

	void setNormal(int index, glm::vec4 newNorm);
	void setNormal(glm::vec4 newNorm[3]);
	void setColor(int index, const TGAColor& newColor);
	void setColor(const TGAColor newColor[3]);

	void setCameraPos(int index, glm::vec4 newVert);
	//void setCameraPos(glm::vec4 newVert[3]);

	//根据y值排序，使得v1.y>v2.y>v3.y
	//void SortVertexsByY();

	//判断三角形的三个点是否共线,共线时返回true
	//bool ThreePointCollinearity();
	//对三角形顶点逆时针排序
	//void EnsureCounterClockwiseOrder();
	

public:
	Vertex vertex[3];
};


#endif // !TRIANGLE



