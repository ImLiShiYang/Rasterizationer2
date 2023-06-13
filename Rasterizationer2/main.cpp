#include <iostream>
#include "Rasterizer.h"
#include <string>

void main()
{
	uint16_t frame = 0;
	float angle = 0;

	while (angle < 360)
	{
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);

		Rasterizer rast(filename, image);
		rast.SetTheta(angle);
		rast.draw();
		rast.output();
		frame++;
		angle++;
	}

}


