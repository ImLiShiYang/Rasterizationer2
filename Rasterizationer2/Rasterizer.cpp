#include "Rasterizer.h"


Rasterizer::Rasterizer(std::string file, TGAImage img):filename(file),image(img), width(img.width()), height(img.height())
			, zneardis(0.1f), zfardis(50), fovY(90), aspect((float)4/3)
{
	image.flip_vertically(); /*������ԭ��λ��ͼ�����½�*/

	xmin = 0;
	xmax = width;
	ymin = 0;
	ymax = height;

	cameraPos = glm::vec3(0, 0, 2);
	theta = 0;
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

void Rasterizer::draw(std::vector<std::shared_ptr<Triangle>>& TriangleList)
{
	glm::mat4 MV = View_Matrix(cameraPos, glm::vec3(0.0f), glm::vec3(0, 1.0f, 0)) * Model_Matrix();
	glm::mat4 P = Perspective_Matrix(zneardis, zfardis, fovY, aspect);


	for (std::shared_ptr<Triangle> t : TriangleList)
	{
		//�Ӿֲ�����ת�����������
		std::vector<glm::vec4> vert
		{
			MV * t->vertex[0].vertex,
			MV * t->vertex[1].vertex,
			MV * t->vertex[2].vertex
		};


		//������ռ�ת������βü��ռ�
		for (int i = 0; i < 3; i++)
		{
			vert[i] = P * vert[i];
		}
		
		//�򵥴������βü���������Χ��������ֱ�Ӷ���
		for (int i = 0; i < 3; i++)
		{
			if (vert[i].w > -zneardis || vert[i].w < -zfardis)
				return;
		}
		std::vector<glm::vec4> clipSpacePos = vert;

		//͸�ӳ������ӿڱ任
		for (int i = 0; i < 3; i++)
		{
			vert[i] /= vert[i].w;
			vert[i] = Viewport_Matrix(width, height) * vert[i];
		}

		Triangle NewTri = *t;
		for (size_t i = 0; i < 3; i++)
		{
			//NewTriangle�Ķ�������Ļ�ռ��µ�����
			NewTri.setVertexPos(i, vert[i]);
			//NewTri.setColor(i, t->vertex[i].vertexColor);
			//NewTri.setNormal(i, normal[i]);
		}


		//��Ļ�ü�
		std::vector<Triangle> NewTriangle = SuthHodgClipTriangle(NewTri, clipSpacePos);
		for (auto sjx : NewTriangle)
			rasterize_wireframe(sjx);
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

//����ü��㷨
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
//����ü�
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
		//�����㶼����
		if (!(code0 | code1))
		{
			f = true;
			break;
		}
		//�����㶼����
		else if ((code0 & code1))
		{
			line = Line();
			break;
		}

		int code = code0 ? code0 : code1;
		//͸�Ӳ�ֵ
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
	//�洢��Ļ���ĸ��սǵ㣬ʹ����ʱ��˳��洢
	std::vector<glm::vec2> screenIntersection = { glm::vec2(0,0),
		glm::vec2(0,height),glm::vec2(width,height),glm::vec2(width,0) };

	//�����ε�������
	std::vector<Vertex> poly_points
	{
		triangle.vertex[0],
		triangle.vertex[1],
		triangle.vertex[2]
	};
		
	//�ü���ĵ�
	for (int i = 0; i < 4; i++)
		SuthHodgClip(poly_points, screenIntersection[i], screenIntersection[(i + 1) % 4], clipSpacePos);

	std::vector<Triangle> triangles;

	if (!poly_points.empty())
	{
		for (int i = 1; i < poly_points.size() - 1; i++)
		{
			triangles.emplace_back(Triangle(poly_points[0], poly_points[i % poly_points.size()],
				poly_points[(i + 1) % poly_points.size()]));
		}
	}
	
	

	return triangles;
}

void Rasterizer::SuthHodgClip(std::vector<Vertex>& poly_points, glm::vec2 p1, glm::vec2 p2, std::vector<glm::vec4>& clipSpacePos)
{
	//�洢���к�ĵ�
	std::vector<Vertex> new_point;
	//�洢���к��Ӧ�ü��ռ�ĵ�
	std::vector<glm::vec4> new_clipSpacePos;

	for (int i = 0; i < poly_points.size(); i++)
	{
		// i �� j �����������γ�һ����
		int j = (i + 1) % poly_points.size();
		float x1 = poly_points[i].vertex.x, y1 = poly_points[i].vertex.y;
		float x2 = poly_points[j].vertex.x, y2 = poly_points[j].vertex.y;

		// �����һ��������ڲü��ߵ�λ��
		//�����Ƕ�ά�����Ĳ�����㣬���ֵ�����㣬˵��(x1,y1)������(p1,p2)����࣬��֮�����Ҳ�
 		float i_pos = (p2.x - p1.x) * (y1 - p1.y) - (p2.y - p1.y) * (x1 - p1.x);

		// ����ڶ���������ڲü��ߵ�λ��
		float j_pos = (p2.x - p1.x) * (y2 - p1.y) - (p2.y - p1.y) * (x2 - p1.x);

		// ���1���������㶼���ڲ�ʱ
		if (i_pos < 0 && j_pos < 0)
		{
			//ֻ��ӵڶ�����
			new_point.push_back(poly_points[j]);
			new_clipSpacePos.emplace_back(clipSpacePos[j]);
		}

		// ���2������һ�������ⲿʱ
		else if (i_pos >= 0 && j_pos < 0)
		{
			// ��ӱ�Ե�Ľ���͵ڶ�����
			//��Ե�Ľ���
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			//��Ļ�ռ�
			float t;
			if (poly_points[j].vertex.x - poly_points[i].vertex.x != 0)
				t = (x - poly_points[i].vertex.x) / (poly_points[j].vertex.x - poly_points[i].vertex.x);
			else
				t = (y - poly_points[i].vertex.y) / (poly_points[j].vertex.y - poly_points[i].vertex.y);
			//͸�ӽ�����ֵ
			float t_perspective = perspectiveLerp(t, clipSpacePos[i], clipSpacePos[j]);
			Vertex v = lerp(poly_points[i], poly_points[j], t_perspective);
			//���㻹������Ļ�ռ�Ĳ�ֵ����
			v.vertex= lerp(poly_points[i].vertex, poly_points[j].vertex, t);
			v.vertex = glm::vec4(x, y, v.vertex.w, 1);
			new_point.push_back(v);

			glm::vec4 v_clipspace = lerp(clipSpacePos[i], clipSpacePos[j], t_perspective);
			new_clipSpacePos.emplace_back(v_clipspace);
			//�ڶ�����
			new_point.push_back(poly_points[j]);
			new_clipSpacePos.emplace_back(clipSpacePos[j]);

		}

		// ���3�����ڶ��������ⲿʱ
		else if (i_pos < 0 && j_pos >= 0)
		{
			// ��ӱ�Ե�Ľ���
			//��Ե�Ľ���
			float x = x_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			float y = y_intersect(p1, p2, poly_points[i].vertex, poly_points[j].vertex);
			//��Ļ�ռ�
			float t;
			if (poly_points[j].vertex.x - poly_points[i].vertex.x != 0)
				t = (x - poly_points[i].vertex.x) / (poly_points[j].vertex.x - poly_points[i].vertex.x);
			else
				t = (y - poly_points[i].vertex.y) / (poly_points[j].vertex.y - poly_points[i].vertex.y);
			//͸�ӽ�����ֵ
			float t_perspective = perspectiveLerp(t, clipSpacePos[i], clipSpacePos[j]);
			Vertex v = lerp(poly_points[i], poly_points[j], t_perspective);
			//���㻹������Ļ�ռ�Ĳ�ֵ����
			v.vertex = lerp(poly_points[i].vertex, poly_points[j].vertex, t);
			v.vertex = glm::vec4(x, y, v.vertex.w, 1);
			new_point.push_back(v);

			glm::vec4 v_clipspace = lerp(clipSpacePos[i], clipSpacePos[j], t_perspective);
			new_clipSpacePos.emplace_back(v_clipspace);
		}

		// ���4���������㶼���ⲿʱ
		else
		{
			// ������κε�
		}
	}

	poly_points = new_point;
	clipSpacePos = new_clipSpacePos;
}

glm::mat4 Rasterizer::Model_Matrix()
{
	glm::mat4 matrix(1.0f);
	float angle = glm::radians(theta);
	glm::vec3 axis(1.0f, 1.0f, 1.0f);
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
