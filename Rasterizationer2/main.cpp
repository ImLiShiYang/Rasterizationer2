#include <iostream>
#include "Rasterizer.h"
#include "OBJ_load.h"
#include <string>
#include <chrono>

auto eye_pos = glm::vec3(0, 0, 2.5f);
auto gaze_dir = glm::vec3(0, 0, 0);
auto view_up = glm::vec3(0, 1, 0);
float angle = 0;
int width = 800;
int height = 600;

float zneardis = 0.1f;
float zfardis = 50;
float fovY = 90;
float aspect = (float)4 / 3;

class Shader :public IShader
{
public:
	Shader() = default;
	virtual void VertexShader(Triangle& primitive)
	{
		modeling = Model_Matrix(glm::vec3(0, -4, 0), angle, glm::vec3(0, 1, 0));
		viewing = View_Matrix(eye_pos, gaze_dir, view_up);
		projection = Perspective_Matrix(zneardis, zfardis, fovY, aspect);

		glm::mat4 MV = viewing * modeling;

		glm::mat4 mvn = viewing * modeling;
		mvn = glm::transpose(glm::inverse(mvn));

		for (Vertex& v : primitive.vertex)
		{
			v.normal = mvn * v.normal;
			v.cameraSpacePos= MV * v.vertex;
			v.worldPos= modeling * v.vertex;
			v.vertex = projection * viewing * modeling * v.vertex;
		}
		tri_cameraspace = primitive;

	}

	virtual TGAColor FragmentShader(Vertex& vertex)
	{
		float Ie = 0;
		glm::vec3 Ka(0.005f), Kd((float)vertex.vertexColor.bgra[2]/255, (float)vertex.vertexColor.bgra[1]/255, (float)vertex.vertexColor.bgra[0]/255),
			Ks(0.7937f), Ke(0.0f);
		glm::vec3 color(0, 0, 0);
		glm::vec3 pos = vertex.cameraSpacePos;
		for (PointLight& light : lights)
		{
			//emission
			auto emission = Ke * Ie;

			//ambiant
			auto ambient = Ka * 0.1f;

			//if (!payload.material.map_Bump.empty())
				//__debugbreak();

			//diffuse
			auto lightdir = glm::normalize(light.position - pos);
			auto r_2 = std::pow(glm::distance(light.position, pos),2.0f);
			auto irradiance = glm::dot(glm::vec3(vertex.normal), lightdir);
			auto diffuse = Kd * std::max(0.0f, irradiance) * light.intensity
				/ r_2;

			//specular
			auto viewdir = glm::normalize(-pos);
			auto h = glm::normalize(viewdir + lightdir);
			auto specular = Ks * light.intensity * std::pow(std::max(0.0f, glm::dot(h, glm::vec3(vertex.normal))), 20.f)
				/ r_2;

			color += emission + ambient + diffuse + specular;
		}
		color = color * 255.0f;

		color.x = color.x > 255.0f ? 255.0f : color.x;
		color.y = color.y > 255.0f ? 255.0f : color.y;
		color.z = color.z > 255.0f ? 255.0f : color.z;

		//if (color.x == 0 && color.y == 0 && color.z == 0)
		   // __debugbreak();

		return TGAColor(color.x, color.y, color.z);
	}

public:
	float theta_uniform;
	std::vector<PointLight> lights;
private:
	Triangle tri_cameraspace;
	glm::mat4 modeling, viewing, projection;
};


void main()
{
	uint16_t frame = 0;
    auto begin = std::chrono::steady_clock::now();

    //std::vector<Triangle> TriangleList;

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

	Shader shader;
	PointLight light = PointLight(glm::vec3(20, 20, 20), 1000);
	light.position = View_Matrix(eye_pos, gaze_dir, view_up) * glm::vec4(light.position, 0);
		
	shader.lights.push_back(light);

	while (angle < 360)
	{
		//eye_pos=SetCamera(eye_pos, angle, glm::vec3(0, 1, 0));
	
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);

		Rasterizer rast(filename, image);
        rast.TurnOnBackCulling();
		rast.draw(MeshList, shader);
		rast.output();

        std::cout << "frame:" << frame << std::endl;
        frame++;
        angle += 5;
	}

}


