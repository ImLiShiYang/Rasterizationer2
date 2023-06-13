#include "Rasterizer.h"

Rasterizer::Rasterizer(std::string file, TGAImage img):filename(file),image(img), width(img.width()), height(img.height())
{
	image.flip_vertically(); /*让坐标原点位于图像左下角*/
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

void Rasterizer::output()
{
	image.write_tga_file(filename + ".tga");
}
