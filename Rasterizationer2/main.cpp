#include <iostream>
#include "Rasterizer.h"
#include <string>
#include <chrono>

void main()
{
	uint16_t frame = 0;
	float angle = 0;
    auto begin = std::chrono::steady_clock::now();

    std::vector<std::shared_ptr<Triangle>> TriangleList;

    
    {
        glm::vec3 v[3];
        /*v[0] = glm::vec3(561, 600, -2);
        v[1] = glm::vec3(498, 600, -2);
        v[2] = glm::vec3(688, 361, -2);*/

        v[0] = glm::vec3(2, 0, -2);
        v[1] = glm::vec3(0, 2, -2);
        v[2] = glm::vec3(-2, 0, -2);

        std::shared_ptr<Triangle> t1(new Triangle(v));
        t1->setColor(0, TGAColor(255, 0, 0));
        t1->setColor(1, TGAColor(0, 255, 0));
        t1->setColor(2, TGAColor(0, 0, 255));
        TriangleList.push_back(t1);
    }
    
    {
        glm::vec3 v[3];
        v[0] = glm::vec3(3.5, -1, -5);
        v[1] = glm::vec3(2.5, 1.5, -5);
        v[2] = glm::vec3(-1, 0.5, -5);
        std::shared_ptr<Triangle> t1(new Triangle(v));
        t1->setColor(0, TGAColor(255, 0, 0));
        t1->setColor(1, TGAColor(0, 255, 0));
        t1->setColor(2, TGAColor(0, 0, 255));
        TriangleList.push_back(t1);
    }

	while (angle < 360)
	{
		
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);

		Rasterizer rast(filename, image);
		rast.SetTheta(angle);
        rast.TurnOnBackCulling();
		rast.draw(TriangleList);
		rast.output();

        std::cout << "frame:" << frame << std::endl;
        frame++;
        angle += 5;
	}

}


