#include "Rasterizer.h"

Rasterizer::Rasterizer(std::string file, TGAImage img):filename(file),image(img), width(img.width()), height(img.height())
{
	image.flip_vertically(); /*让坐标原点位于图像左下角*/

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

void Rasterizer::draw()
{
	glm::mat4 MVP = Perspective_Matrix(0.01f, 100.f, 90.f, (float)width / height) *
		View_Matrix(cameraPos, glm::vec3(0.0f), glm::vec3(0, 1.0f, 0)) * Model_Matrix();

	Triangle tri(glm::vec4(0.5f, -0.5f, 0.5f, 1.f), glm::vec4(-0.5f, -0.5f, -0.5f, 1.f), glm::vec4(-0.5f, -0.5f, 0.5f, 1.f));
	for (int i = 0; i < 3; i++)
	{
		tri.vertex[i] = MVP * tri.vertex[i];
		tri.vertex[i] /= tri.vertex[i].w;
		tri.vertex[i] = Viewport_Matrix(width, height) * tri.vertex[i];
	}

	tri.setColor(0, TGAColor(255, 0, 0, 0));
	tri.setColor(1, TGAColor(0, 255, 0, 0));
	tri.setColor(2, TGAColor(0, 0, 255, 0));

	rasterize_wireframe(tri);
}

void Rasterizer::output()
{
	image.write_tga_file(filename + ".tga");
}

void Rasterizer::rasterize_wireframe(const Triangle& t)
{
	draw_line(t.vertex[0], t.vertex[1]);
	draw_line(t.vertex[1], t.vertex[2]);
	draw_line(t.vertex[2], t.vertex[0]);
}

glm::mat4 Rasterizer::Model_Matrix()
{
	glm::mat4 matrix(1.0f);
	float angle = glm::radians(theta);
	glm::vec3 axis(0.0f, 1.0f, 1.0f);
	matrix = glm::rotate(matrix, angle, axis);

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
