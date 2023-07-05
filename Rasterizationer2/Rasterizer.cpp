#include "Rasterizer.h"
#include <iostream>


Rasterizer::Rasterizer(std::string file, TGAImage img):filename(file),image(img), width(img.width()), height(img.height())
{
	image.flip_vertically(); /*让坐标原点位于图像左下角*/
	color_buffer = TGAImage(width * 2, height * 2, TGAImage::RGB);
	xmin = 0;
	xmax = width;
	ymin = 0;
	ymax = height;

	z_buffer.resize(img.width() * img.height() * 4);
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
			image.set(y, x, white);
		else
			image.set(x, y, white);
	}
}

void Rasterizer::MVP_Matrix()
{
	
}

//void Rasterizer::SetTheta(float t)
//{
//	theta = t;
//}

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

Vertex Rasterizer::MSAA(const Vertex& v)
{

	return Vertex();
}

void Rasterizer::rasterize_edge_equation(const Triangle& origin_m,const Triangle& m, std::vector<glm::vec4>& clipSpacePos, IShader& shader)
{
	int leftEdge = std::floor(std::min({ m.vertex[0].vertex.x, m.vertex[1].vertex.x, m.vertex[2].vertex.x }));
	int rightEdge = std::ceil(std::max({ m.vertex[0].vertex.x, m.vertex[1].vertex.x, m.vertex[2].vertex.x }));
	int bottomEdge = std::floor(std::min({ m.vertex[0].vertex.y, m.vertex[1].vertex.y, m.vertex[2].vertex.y }));
	int TopEdge = std::ceil(std::max({ m.vertex[0].vertex.y, m.vertex[1].vertex.y, m.vertex[2].vertex.y }));
	 
	for (int y = bottomEdge; y < TopEdge; y+=2) 
	{
		for (int x = leftEdge; x < rightEdge; x+=2) 
		{
			Vertex pos[2][2];
			for (int a : {x, x+1})
			{
				for (int b : {y, y+1})
				{
					if (insideTriangle(m, (float)a+0.5f, (float)b+0.5f))
					{
						pos[a-x][b-y] = barycentric_coordinates_perspective(glm::vec2((float)a + 0.5f, (float)b + 0.5f), origin_m.vertex[0],
							origin_m.vertex[1], origin_m.vertex[2], clipSpacePos);
					}
				}
			}

			float ddx = -1;  // 初始化ddx
			float ddy = -1;  // 初始化ddy

			for (int i : {0, 1})
			{
				if (!pos[i][0].empty() && !pos[i][1].empty())
				{
					ddx = std::max(std::fabs(glm::distance(pos[i][0].texcoord, pos[i][1].texcoord)), ddx);
				}
				if (!pos[0][i].empty() && !pos[1][i].empty())
				{
					ddy = std::max(std::fabs(glm::distance(pos[0][i].texcoord, pos[1][i].texcoord)), ddy);
				}
			}

			// 设置纹理坐标的梯度
			shader.setddx(ddx);
			shader.setddy(ddy);

			
			// 对于2x2矩阵中的每个像素
			for (int a : {0, 1})
			{
				for (int b : {0, 1})
				{
					glm::vec2 pos_((float)x + a, (float)y + b);
					glm::vec2 p[4];
					glm::vec2 p1(pos_.x + 0.25f, pos_.y + 0.25f), p2(pos_.x + 0.75f, pos_.y + 0.25f);
					glm::vec2 p3(pos_.x + 0.25f, pos_.y + 0.75f), p4(pos_.x + 0.75f, pos_.y + 0.75f);
					p[0] = p1;
					p[1] = p2;
					p[2] = p3;
					p[3] = p4;

					TGAColor finalcolor;
					bool first_sub = false;

					for (int i = 0; i < 4; i++)
					{
						if (insideTriangle(m, p[i].x, p[i].y))
						{
							Vertex pixel_sub = barycentric_coordinates_perspective(p[i], origin_m.vertex[0], origin_m.vertex[1], origin_m.vertex[2], clipSpacePos);
							if (pixel_sub.vertex.z > z_buffer[get_index((p[i].x - 0.25f) * 2, (p[i].y - 0.25f) * 2)])
							{
								if (!first_sub)
								{
									finalcolor = shader.FragmentShader(pixel_sub);
									first_sub = true;
								}
								z_buffer[get_index((p[i].x - 0.25f) * 2, (p[i].y - 0.25f) * 2)] = pixel_sub.vertex.z;
								color_buffer.set((p[i].x - 0.25f) * 2, (p[i].y - 0.25f) * 2, finalcolor);
							}

						}
					}

				}
			}
		}
	}
}



int Rasterizer::get_index(int x, int y)
{
	return y * width + x;
}

void Rasterizer::draw(std::vector<std::shared_ptr<Mesh>> MeshList, IShader& shader)
{

	for (std::shared_ptr<Mesh> m : MeshList)
	{
		for (Triangle t : m->primitives)
		{
			//Triangle t = *m;
			//顶点着色器，这里对三角形的顶点变换为裁剪空间
			shader.VertexShader(t);
			//从局部坐标转换到相机坐标
			std::vector<glm::vec4> vert
			{
				t.vertex[0].vertex,
				t.vertex[1].vertex,
				t.vertex[2].vertex
			};
			
			//背面裁剪
			if (backCulling)
			{
				glm::vec4 v1 = t.vertex[1].cameraSpacePos - t.vertex[0].cameraSpacePos;
				glm::vec4 v2 = t.vertex[2].cameraSpacePos - t.vertex[1].cameraSpacePos;
				glm::vec3 v = glm::cross(glm::vec3(v1), glm::vec3(v2));
				glm::vec3 gaze(t.vertex[0].cameraSpacePos);
				if (vertexOrder == TriangleVertexOrder::counterclockwise)
				{
					if (glm::dot(v, gaze) >= 0)
						continue;
				}
				else
				{
					if (glm::dot(v, gaze) <= 0)
						continue;
				}
			}

			//简单处理的齐次裁剪，超出范围的三角形直接丢弃
			for (int i = 0; i < 3; i++)
			{
				if (vert[i].w > -0.1f || vert[i].w < -50.f)
					continue;
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

			Triangle NewTri = t;
			for (size_t i = 0; i < 3; i++)
			{
				//NewTriangle的顶点是屏幕空间下的坐标
				NewTri.setVertexPos(i, vert[i]);
				NewTri.setColor(i, t.vertex[i].vertexColor);
				NewTri.setNormal(i, t.vertex[i].normal);
				NewTri.setCameraPos(i, t.vertex[i].cameraSpacePos);
			}

			shader.setmtl(m->material);

			//屏幕裁剪
			std::vector<Triangle> NewTriangle = SuthHodgClipTriangle(NewTri, clipSpacePos);
			for (int i = 0; i < NewTriangle.size(); i++)
			{
				Triangle new_tri = NewTriangle[i];

				rasterize_edge_equation(NewTri,new_tri, clipSpacePos, shader);
				//rasterize_wireframe(new_tri);
				//rasterize_edge_walking(sjx, clipSpacePos_Array);
			}
		}
	}
	
	for (int y = 0; y < image.height(); y++)
	{
		for (int x = 0; x < image.width(); x++)
		{
			TGAColor sub1 = color_buffer.get(2 * x, 2 * y) * 0.25f;
			TGAColor sub2 = color_buffer.get(2 * x + 1, 2 * y) * 0.25f;
			TGAColor sub3 = color_buffer.get(2 * x, 2 * y + 1) * 0.25f;
			TGAColor sub4 = color_buffer.get(2 * x + 1, 2 * y + 1) * 0.25f;
			TGAColor final_color = sub1 + sub2 + sub3 + sub4;
			image.set(x, y, final_color);
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
		SuthHodgClip(poly_points, triangle, screenIntersection[i], screenIntersection[(i + 1) % 4], clipSpacePos);

	std::vector<Triangle> triangles;

	if (!poly_points.empty())
	{
		for (int i = 1; i < poly_points.size()-1; i++)
		{
			triangles.emplace_back(Triangle(poly_points[0], poly_points[(i) % poly_points.size()],
				poly_points[(i + 1) % poly_points.size()]));
		}
	}
	
	

	return triangles;
}

void Rasterizer::SuthHodgClip(std::vector<Vertex>& poly_points, Triangle& triangle, glm::vec2 p1, glm::vec2 p2, std::vector<glm::vec4>& clipSpacePos)
{
	//存储裁切后的点
	std::vector<Vertex> new_point;
	//存储裁切后对应裁剪空间的点

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
		}

		// 情况2：仅第一个点在外部时
		else if (i_pos >= 0 && j_pos < 0)
		{
			// 添加边缘的交点和第二个点
			//边缘的交点
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			glm::vec2 pos_intersection(x, y);
			Vertex pos_inter = barycentric_coordinates_perspective(pos_intersection, triangle.vertex[0],
				triangle.vertex[1], triangle.vertex[2], clipSpacePos);
			pos_inter.vertex.x = x;
			pos_inter.vertex.y = y;
			new_point.push_back(pos_inter);

			//第二个点
			new_point.push_back(poly_points[j]);

		}

		// 情况3：仅第二个点在外部时
		else if (i_pos < 0 && j_pos >= 0)
		{
			// 添加边缘的交点
			//边缘的交点
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			glm::vec2 pos_intersection(x, y);
			Vertex pos_inter = barycentric_coordinates_perspective(pos_intersection, triangle.vertex[0],
				triangle.vertex[1], triangle.vertex[2], clipSpacePos);
			pos_inter.vertex.x = x;
			pos_inter.vertex.y = y;
			new_point.push_back(pos_inter);
		}

		// 情况4：当两个点都在外部时
		else
		{
			// 不添加任何点
		}
	}

	poly_points = new_point;
}

