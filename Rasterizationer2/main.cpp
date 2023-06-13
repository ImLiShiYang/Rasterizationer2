#include <iostream>
#include "Rasterizer.h"
#include <string>

void main()
{
	uint16_t frame = 0;
	float angle = 0;

	while (frame < 100)
	{
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);

		glm::vec3 v0(20, 30, 0), v1(300, 400, 0);
		

		Rasterizer rast(filename, image);
		rast.draw_line(v0, v1);
		rast.output();
		frame++;
	}

}


