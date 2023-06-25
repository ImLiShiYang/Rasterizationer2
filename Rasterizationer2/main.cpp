#include <iostream>
#include "Rasterizer.h"
#include "OBJ_load.h"
#include <string>
#include <chrono>

void main()
{
	uint16_t frame = 0;
	float angle = 0;
    auto begin = std::chrono::steady_clock::now();

    std::vector<Triangle> TriangleList;

    std::vector<std::shared_ptr<Mesh>> MeshList;
    Model model("obj/mary/", "Marry.obj");
    //Model model("obj/fighter/", "fighter.obj");
    for (auto& o : model.objects)
    {
        for (auto& m : o.meshes)
        {
            std::shared_ptr<Mesh> m1 = std::make_shared<Mesh>(m);
            MeshList.push_back(m1);
        }
    }

	while (angle < 360)
	{
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);

		Rasterizer rast(filename, image);
		rast.SetTheta(angle);
        rast.SetRotateAxis(glm::vec3(0, 1, 0));
        rast.SetCamera(glm::vec3(0, 0.5, 2.2));
        rast.TurnOnBackCulling();
		rast.draw(MeshList);
		rast.output();

        std::cout << "frame:" << frame << std::endl;
        frame++;
        angle += 5;
	}

}


