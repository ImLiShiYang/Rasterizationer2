#ifndef RASTER_H
#define RASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tgaimage.h"

class Rasterizer
{
public:
	Rasterizer(std::string file, TGAImage img);
	void draw_line(glm::vec3, glm::vec3);
	void output();

private:
	TGAImage image;
	const TGAColor white = TGAColor(255, 255, 255, 255);
	const TGAColor red = TGAColor(255, 0, 0, 255);
	std::string filename;
	int width, height;
};


#endif // !RASTER_H



