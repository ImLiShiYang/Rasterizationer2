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
	Vertex vertex[3];
};

inline Vertex barycentricPerspectiveLerp(const Triangle& t, const glm::vec2& pixel)
{
	Vertex v1 = t.vertex[0]; Vertex v2 = t.vertex[1]; Vertex v3 = t.vertex[2];
	float xa = v1.vertex.x; float ya = v1.vertex.y;
	float xb = v2.vertex.x; float yb = v2.vertex.y;
	float xc = v3.vertex.x; float yc = v3.vertex.y;
	float x = pixel.x; float y = pixel.y;

	float gamma = ((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) /
		((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
	float beta = ((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) /
		((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
	float alpha = 1 - beta - gamma;

	return v1 * alpha + v2 * beta + v3 * gamma;
}


#endif // !TRIANGLE



