#include "Rasterizer.h"


Rasterizer::Rasterizer(std::string file, TGAImage img):filename(file),image(img), width(img.width()), height(img.height())
			, zneardis(0.1f), zfardis(50), fovY(90), aspect((float)4/3)
{
	image.flip_vertically(); /*让坐标原点位于图像左下角*/

	xmin = 0;
	xmax = width;
	ymin = 0;
	ymax = height;

	cameraPos = glm::vec3(0, 0, 2);
	theta = 0;

	z_buffer.resize(img.width() * img.height());
	std::fill(z_buffer.begin(), z_buffer.end(), -std::numeric_limits<float>::infinity());
}

void Rasterizer::draw_line(glm::vec3 v0, glm::vec3 v1)
{
	int x0 = v0.x, y0 = v0.y, x1 = v1.x, y1 = v1.y;
	bool flag = false;

	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		flag = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	for (int x = x0; x <= x1; x++)
	{
		float t = (float)(x - x0) / (x1 - x0);
		int y = y0 + t * (y1 - y0);
		if (flag)
			image.set(y, x, red);
		else
			image.set(x, y, red);
	}
}

void Rasterizer::MVP_Matrix()
{
	
}

void Rasterizer::SetTheta(float t)
{
	theta = t;
}

void Rasterizer::TurnOnBackCulling()
{
	this->backCulling = true;
}

void Rasterizer::TurnOffBackCulling()
{
	this->backCulling = false;
}

void Rasterizer::SetVertexOrder(const TriangleVertexOrder& t)
{
	this->vertexOrder = t;
}

TriangleVertexOrder Rasterizer::GetVertexOrder()
{
	auto v = this->vertexOrder;
	return v;
}

//左上角规则
static float TopLeftRules(const float side, const glm::vec4& v1, const glm::vec4& v2, const TriangleVertexOrder& Order)
{
	if (Order == TriangleVertexOrder::counterclockwise)
	{
		if (std::fabs(side) < 1e-6)
		{
			//在三角形逆序的情况下，v1.y > v2.y 说明是左边
			//v1.y == v2.y && v1.x > v2.x说明是在上边
			return (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x)) ? 0 : -1;
		}
		else
		{
			return side;
		}
	}
	else
	{
		if (std::fabs(side) < 1e-6)
		{
			//顺时针情况下
			return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x)) ? 0 : -1;
		}
		else
		{
			return side;
		}
	}
}

bool Rasterizer::insideTriangle(const Triangle& m, const float x, const float y)
{
	glm::vec4 v1 = m.vertex[0].vertex;
	glm::vec4 v2 = m.vertex[1].vertex;
	glm::vec4 v3 = m.vertex[2].vertex;
	//还是二维向量叉积原理
	float side1 = (v2.y - v1.y) * x + (v1.x - v2.x) * y + v2.x * v1.y - v1.x * v2.y;
	float side2 = (v3.y - v2.y) * x + (v2.x - v3.x) * y + v3.x * v2.y - v2.x * v3.y;
	float side3 = (v1.y - v3.y) * x + (v3.x - v1.x) * y + v1.x * v3.y - v3.x * v1.y;

	side1 = TopLeftRules(side1, v1, v2, vertexOrder);
	side2 = TopLeftRules(side2, v2, v3, vertexOrder);
	side3 = TopLeftRules(side3, v3, v1, vertexOrder);
	//保证顺时针和逆时针两种情况
	return (side1 >= 0 && side2 >= 0 && side3 >= 0) || (side1 <= 0 && side2 <= 0 && side3 <= 0);
}

//const std::array<glm::vec4, 3>& clipSpacePos
void Rasterizer::rasterize_edge_walking(const Triangle& m , const std::array<glm::vec4, 3>& clipSpacePos_Array)
{
	Triangle t = m;
	std::array<glm::vec4, 3> clipSpacePos = clipSpacePos_Array;
	if (t.vertex[0].vertex.y > t.vertex[1].vertex.y)
	{
		std::swap(t.vertex[0], t.vertex[1]);
		std::swap(clipSpacePos[0], clipSpacePos[1]);
	}
	if (t.vertex[0].vertex.y > t.vertex[2].vertex.y)
	{
		std::swap(t.vertex[0], t.vertex[2]);
		std::swap(clipSpacePos[0], clipSpacePos[2]);
	}
	if (t.vertex[1].vertex.y > t.vertex[2].vertex.y)
	{
		std::swap(t.vertex[1], t.vertex[2]);
		std::swap(clipSpacePos[1], clipSpacePos[2]);
	}

	if (t.vertex[0].vertex.y == t.vertex[2].vertex.y)
		return;

	float longEdge = t.vertex[2].vertex.y - t.vertex[0].vertex.y;

	//这里的ceil是为了四舍五入
	for (int y = std::ceil(t.vertex[0].vertex.y - 0.5f); y < std::ceil(t.vertex[1].vertex.y - 0.5f); y++)
	{
		float shortEdge = t.vertex[1].vertex.y - t.vertex[0].vertex.y;

		float shortlerp = ((float)y + 0.5f - t.vertex[0].vertex.y) / shortEdge;
		float longlerp= ((float)y + 0.5f - t.vertex[0].vertex.y) / longEdge;

		Vertex shortVert = lerp(t.vertex[0], t.vertex[1], shortlerp);
		Vertex longVert = lerp(t.vertex[0], t.vertex[2], longlerp);

		Vertex shortVert_per = perspectiveLerp(t.vertex[0], t.vertex[1], shortlerp, clipSpacePos[0], clipSpacePos[1]);
		Vertex longVert_per = perspectiveLerp(t.vertex[0], t.vertex[2], shortlerp, clipSpacePos[0], clipSpacePos[2]);

		if (shortVert.vertex.x > longVert.vertex.x)
		{
			std::swap(shortVert, longVert);
			//std::swap(shortVert_per, longVert_per);
		}

		for (float x = std::ceil(shortVert.vertex.x-0.5f); x < std::ceil(longVert.vertex.x-0.5); x++)
		{
			float pixellerp = ((float)x + 0.5f - shortVert.vertex.x) / (longVert.vertex.x - shortVert.vertex.x);
			Vertex pixel = perspectiveLerp(shortVert, longVert, pixellerp, shortVert_per.vertex, longVert_per.vertex);

			image.set(x, y, pixel.vertexColor);    
		}
	}

	for (float y = std::ceil(t.vertex[1].vertex.y - 0.5f); y < std::ceil(t.vertex[2].vertex.y - 0.5f); y++)
	{
		float shortEdge = t.vertex[2].vertex.y - t.vertex[1].vertex.y;

		float shortlerp = ((float)y + 0.5f - t.vertex[1].vertex.y) / shortEdge;
		float longlerp = ((float)y + 0.5f - t.vertex[0].vertex.y) / longEdge;

		Vertex shortVert = lerp(t.vertex[1], t.vertex[2], shortlerp);
		Vertex longVert = lerp(t.vertex[0], t.vertex[2], longlerp);

		Vertex shortVert_per = perspectiveLerp(t.vertex[0], t.vertex[1], shortlerp, clipSpacePos[0], clipSpacePos[1]);
		Vertex longVert_per = perspectiveLerp(t.vertex[0], t.vertex[2], shortlerp, clipSpacePos[0], clipSpacePos[2]);

		if (shortVert.vertex.x > longVert.vertex.x)
		{
			std::swap(shortVert, longVert);
			//std::swap(shortVert_per, longVert_per);
		}

		for (float x = std::ceil(shortVert.vertex.x - 0.5f); x < std::ceil(longVert.vertex.x - 0.5); x++)
		{
			float pixellerp = ((float)x + 0.5f - shortVert.vertex.x) / (longVert.vertex.x - shortVert.vertex.x);
			Vertex pixel = perspectiveLerp(shortVert, longVert, pixellerp, shortVert_per.vertex, longVert_per.vertex);

			image.set(x, y, pixel.vertexColor);
		}
	}

}

void Rasterizer::rasterize_edge_equation(const Triangle& m, std::vector<glm::vec4>& clipSpacePos)
{
	int leftEdge = std::floor(std::min({ m.vertex[0].vertex.x, m.vertex[1].vertex.x, m.vertex[2].vertex.x }));
	int rightEdge = std::ceil(std::max({ m.vertex[0].vertex.x, m.vertex[1].vertex.x, m.vertex[2].vertex.x }));
	int bottomEdge = std::floor(std::min({ m.vertex[0].vertex.y, m.vertex[1].vertex.y, m.vertex[2].vertex.y }));
	int TopEdge = std::ceil(std::max({ m.vertex[0].vertex.y, m.vertex[1].vertex.y, m.vertex[2].vertex.y }));
	 
	for (int y = bottomEdge; y < TopEdge; y++) 
	{
		for (int x = leftEdge; x < rightEdge; x++) 
		{
			Vertex pos(glm::vec4((float)x + 0.5, (float)y + 0.5, 0.f, 0.f));
			if (insideTriangle(m, pos.vertex.x, pos.vertex.y))
			{
				//Vertex pixel= barycentric_coordinates(pos, m.vertex[0], m.vertex[1], m.vertex[2]);
				Vertex pixel = barycentric_coordinates_perspective(pos, m.vertex[0], m.vertex[1], m.vertex[2], clipSpacePos);
				if (pixel.vertex.z > z_buffer[get_index(pos.vertex.x, pos.vertex.y)])
				{
					z_buffer[get_index(pos.vertex.x, pos.vertex.y)] = pixel.vertex.z;
					image.set(x, y, pixel.vertexColor);
				}
			}
		}
	}
}

int Rasterizer::get_index(int x, int y)
{
	return y * width + x;
}

void Rasterizer::draw(std::vector<std::shared_ptr<Triangle>>& TriangleList)
{
	glm::mat4 MV = View_Matrix(cameraPos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 1.0f, 0)) * Model_Matrix();
	glm::mat4 P = Perspective_Matrix(zneardis, zfardis, fovY, aspect);


	for (std::shared_ptr<Triangle> t : TriangleList)
	{
		/**/
		//从局部坐标转换到相机坐标
		std::vector<glm::vec4> vert
		{
			MV * t->vertex[0].vertex,
			MV * t->vertex[1].vertex,
			MV * t->vertex[2].vertex
		};

		if (backCulling)
		{
			glm::vec4 v1 = vert[1] - vert[0], v2 = vert[2] - vert[1];
			glm::vec3 v = glm::cross(glm::vec3(v1), glm::vec3(v2));
			glm::vec3 gaze(vert[0]);
			if (vertexOrder == TriangleVertexOrder::counterclockwise)
			{
				if (glm::dot(v, gaze) >= 0)
					return;
			}
			else
			{
				if (glm::dot(v, gaze) <= 0)
					return;
			}
		}

		//从相机空间转换到齐次裁剪空间
		for (int i = 0; i < 3; i++)
		{
			vert[i] = P * vert[i];
		}
		
		//简单处理的齐次裁剪，超出范围的三角形直接丢弃
		for (int i = 0; i < 3; i++)
		{
			if (vert[i].w > -zneardis || vert[i].w < -zfardis)
				return;
		}
		std::vector<glm::vec4> clipSpacePos = vert;

		//透视除法
		for (int i = 0; i < 3; i++)
		{
			vert[i] /= vert[i].w;
		}
		//视口变换
		for (int i = 0; i < 3; i++)
		{
			vert[i] = Viewport_Matrix(width, height) * vert[i];
		}
		
		Triangle NewTri = *t;
		for (size_t i = 0; i < 3; i++)
		{
			//NewTriangle的顶点是屏幕空间下的坐标
			NewTri.setVertexPos(i, vert[i]);
			NewTri.setColor(i, t->vertex[i].vertexColor);
			//NewTri.setNormal(i, normal[i]);
		}

		//std::array<glm::vec4, 3> clipSpacePos_Array;

		//屏幕裁剪
		std::vector<Triangle> NewTriangle = SuthHodgClipTriangle(NewTri, clipSpacePos);
		for (int i=0;i< NewTriangle.size();i++)
		{
			Triangle new_tri = NewTriangle[i];
			//rasterize_wireframe(sjx);
			//rasterize_edge_walking(sjx, clipSpacePos_Array);
			std::vector<glm::vec4> v;
			if (!clipSpacePos.empty())
			{
				v.emplace_back(clipSpacePos[0]);
				v.emplace_back(clipSpacePos[(i + 1) % clipSpacePos.size()]);
				v.emplace_back(clipSpacePos[(i + 2) % clipSpacePos.size()]);
			}

			rasterize_edge_equation(new_tri, v);
		}
	}

}

void Rasterizer::output()
{
	image.write_tga_file(filename + ".tga");
}

void Rasterizer::rasterize_wireframe(const Triangle& t)
{
	draw_line(t.vertex[0].vertex, t.vertex[1].vertex);
	draw_line(t.vertex[1].vertex, t.vertex[2].vertex);
	draw_line(t.vertex[2].vertex, t.vertex[0].vertex);
}

//编码裁剪算法
std::vector<Line> Rasterizer::clip_Cohen_Sutherland(const Triangle& t, std::vector<glm::vec4> clipSpacePos)
{
	auto vert = t.vertex;
	std::vector<Line> line
	{
		Line(vert[0],vert[1]),
		Line(vert[1],vert[2]),
		Line(vert[2],vert[0])
	};

	for (int i = 0; i < 3; i++)
	{
		CodeClip(line[i], { clipSpacePos[i],clipSpacePos[(i + 1) % 3] });
	}

	return line;
}

int Rasterizer::GetCode(float x, float y)
{
	int code = 0;
	if (x < xmin)
		code |= LEFT;
	else if (x > xmax)
		code |= RIGHT;

	if (y < ymin)
		code |= BOTTOM;
	else if (y > ymax)
		code |= TOP;

	return code;
}
//编码裁剪
void Rasterizer::CodeClip(Line& line, std::vector<glm::vec4> clipSpacePos)
{
	float& x0 = line.v1.vertex.x, & y0 = line.v1.vertex.y, & z0 = line.v1.vertex.z;
	float& x1 = line.v2.vertex.x, & y1 = line.v2.vertex.y, & z1 = line.v2.vertex.z;

	int code0 = GetCode(x0, y0);
	int code1 = GetCode(x1, y1);

	bool f = false;
	float x, y, z;

	while (1)
	{
		//两个点都在内
		if (!(code0 | code1))
		{
			f = true;
			break;
		}
		//两个点都在外
		else if ((code0 & code1))
		{
			line = Line();
			break;
		}

		int code = code0 ? code0 : code1;
		//透视插值
		if (code & LEFT)
		{
			float t = (xmin - x0) / (x1 - x0);
			t = perspectiveLerp(t, clipSpacePos[0], clipSpacePos[1]);
			y = y0 + t * (y1 - y0);
			z = z0 + t * (z1 - z0);
			x = xmin;
		}
		else if (code & RIGHT)
		{
			float t = (xmax - x0) / (x1 - x0);
			t = perspectiveLerp(t, clipSpacePos[0], clipSpacePos[1]);
			y = y0 + t * (y1 - y0);
			z = z0 + t * (z1 - z0);
			x = xmax;
		}
		else if (code & BOTTOM)
		{
			float t = (ymin - y0) / (y1 - y0);
			t = perspectiveLerp(t, clipSpacePos[0], clipSpacePos[1]);
			x = x0 + (x1 - x0) * t;
			z = z0 + t * (z1 - z0);
			y = ymin;
		}
		else if (code & TOP)
		{
			float t = (ymax - y0) / (y1 - y0);
			x = x0 + (x1 - x0) * t;
			z = z0 + t * (z1 - z0);
			y = ymax;
		}

		if (code == code0)
		{
			x0 = x;
			y0 = y;
			code0 = GetCode(x, y);
		}
		else if (code == code1)
		{
			x1 = x;
			y1 = y;
			code1 = GetCode(x, y);
		}

	}
}

std::vector<Triangle> Rasterizer::SuthHodgClipTriangle(Triangle& triangle, std::vector<glm::vec4>& clipSpacePos)
{
	//存储屏幕的四个拐角点，使用逆时针顺序存储
	std::vector<glm::vec2> screenIntersection = { glm::vec2(0,0),
		glm::vec2(0,height),glm::vec2(width,height),glm::vec2(width,0) };

	//三角形的三个点
	std::vector<Vertex> poly_points
	{
		triangle.vertex[0],
		triangle.vertex[1],
		triangle.vertex[2]
	};

	//裁剪后的点
	for (int i = 0; i < 4; i++)
		SuthHodgClip(poly_points, screenIntersection[i], screenIntersection[(i + 1) % 4], clipSpacePos);

	std::vector<Triangle> triangles;

	if (!poly_points.empty())
	{
		for (int i = 0; i < poly_points.size(); i++)
		{
			triangles.emplace_back(Triangle(poly_points[0], poly_points[(i + 1) % poly_points.size()],
				poly_points[(i + 2) % poly_points.size()]));
		}
	}
	
	

	return triangles;
}

void Rasterizer::SuthHodgClip(std::vector<Vertex>& poly_points, glm::vec2 p1, glm::vec2 p2, std::vector<glm::vec4>& clipSpacePos)
{
	//存储裁切后的点
	std::vector<Vertex> new_point;
	//存储裁切后对应裁剪空间的点
	std::vector<glm::vec4> new_clipSpacePos;

	for (int i = 0; i < poly_points.size(); i++)
	{
		// i 和 j 在三角形中形成一条线
		int j = (i + 1) % poly_points.size();
		float x1 = poly_points[i].vertex.x, y1 = poly_points[i].vertex.y;
		float x2 = poly_points[j].vertex.x, y2 = poly_points[j].vertex.y;

		// 计算第一个点相对于裁剪线的位置
		//这里是二维向量的叉积运算，如果值大于零，说明(x1,y1)在向量(p1,p2)的左侧，反之则是右侧
 		float i_pos = (p2.x - p1.x) * (y1 - p1.y) - (p2.y - p1.y) * (x1 - p1.x);

		// 计算第二个点相对于裁剪线的位置
		float j_pos = (p2.x - p1.x) * (y2 - p1.y) - (p2.y - p1.y) * (x2 - p1.x);

		// 情况1：当两个点都在内部时
		if (i_pos < 0 && j_pos < 0)
		{
			//只添加第二个点
			new_point.push_back(poly_points[j]);
			new_clipSpacePos.emplace_back(clipSpacePos[j]);
		}

		// 情况2：仅第一个点在外部时
		else if (i_pos >= 0 && j_pos < 0)
		{
			// 添加边缘的交点和第二个点
			//边缘的交点
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			//屏幕空间
			float t;
			if (poly_points[j].vertex.x - poly_points[i].vertex.x != 0)
				t = (x - poly_points[i].vertex.x) / (poly_points[j].vertex.x - poly_points[i].vertex.x);
			else
				t = (y - poly_points[i].vertex.y) / (poly_points[j].vertex.y - poly_points[i].vertex.y);
			//透视矫正插值
			float t_perspective = perspectiveLerp(t, clipSpacePos[i], clipSpacePos[j]);
			Vertex v = lerp(poly_points[i], poly_points[j], t_perspective);
			//顶点还是用屏幕空间的插值计算
			//v.vertex= lerp(poly_points[i].vertex, poly_points[j].vertex, t);
			v.vertex = glm::vec4(x, y, v.vertex.w, 1);
			new_point.push_back(v);

			glm::vec4 v_clipspace = lerp(clipSpacePos[i], clipSpacePos[j], t_perspective);
			new_clipSpacePos.emplace_back(v_clipspace);
			//第二个点
			new_point.push_back(poly_points[j]);
			new_clipSpacePos.emplace_back(clipSpacePos[j]);

		}

		// 情况3：仅第二个点在外部时
		else if (i_pos < 0 && j_pos >= 0)
		{
			// 添加边缘的交点
			//边缘的交点
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			//屏幕空间
			float t;
			if (poly_points[j].vertex.x - poly_points[i].vertex.x != 0)
				t = (x - poly_points[i].vertex.x) / (poly_points[j].vertex.x - poly_points[i].vertex.x);
			else
				t = (y - poly_points[i].vertex.y) / (poly_points[j].vertex.y - poly_points[i].vertex.y);
			//透视矫正插值
			float t_perspective = perspectiveLerp(t, clipSpacePos[i], clipSpacePos[j]);
			Vertex v = lerp(poly_points[i], poly_points[j], t_perspective);
			//顶点还是用屏幕空间的插值计算
			//v.vertex = lerp(poly_points[i].vertex, poly_points[j].vertex, t);
			v.vertex = glm::vec4(x, y, v.vertex.w, 1);
			new_point.push_back(v);

			glm::vec4 v_clipspace = lerp(clipSpacePos[i], clipSpacePos[j], t_perspective);
			new_clipSpacePos.emplace_back(v_clipspace);
		}

		// 情况4：当两个点都在外部时
		else
		{
			// 不添加任何点
		}
	}

	poly_points = new_point;
	clipSpacePos = new_clipSpacePos;
}

glm::mat4 Rasterizer::Model_Matrix()
{
	glm::mat4 matrix(1.0f);
	float angle = glm::radians(theta);
	glm::vec3 axis(1.0f, 1.0f, 0.0f);
	matrix = glm::rotate(matrix, angle, axis);
	//matrix = glm::translate(matrix, glm::vec3(5, 0, 0));

	return matrix;
}

glm::mat4 Rasterizer::View_Matrix(glm::vec3 cameraPos,glm::vec3 center,glm::vec3 up)
{
	glm::vec3 z_vector = glm::normalize(cameraPos - center);
	glm::vec3 x_vector = glm::normalize(glm::cross(up, z_vector));
	glm::vec3 y_vector= glm::normalize(glm::cross(z_vector, x_vector));

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

glm::mat4 Rasterizer::Perspective_Matrix(float zneardis, float zfardis, float fovY, float aspect)
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

glm::mat4 Rasterizer::Orthographic_Matrix(float left, float bottom, float near, float right, float top, float far)
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

glm::mat4 Rasterizer::Viewport_Matrix(float width, float height)
{
	glm::mat4 matrix(1.0f);
	matrix[0][0] = width / 2;
	matrix[1][1] = height / 2;
	matrix[3][0] = width / 2;
	matrix[3][1] = height / 2;

	return matrix;
}
