#include <iostream>
#include "Rasterizer.h"
#include "OBJ_load.h"
#include <string>
#include <chrono>

auto eye_pos = glm::vec3(0, 0.5, 1.5f);
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

	glm::vec3 sample(const glm::vec2 texcoord, const std::vector<TGAImage>& texture)
	{
		// �������������Ϊ��
		if (!texture.empty())
		{
			// ��ʼ��mipmap�ȼ�Ϊ0
			float mipmap_level = 0;

			// ������������Ĳ��ֵ����
			if (ddx != -1.0f || ddy != -1.0f)
			{
				// ���㵱ǰ����mipmap�ȼ�
				float max_level = texture.size() - 1;

				// ���ݺ����������ֵ����Ԥ�ڵ�mipmap�ȼ�
				float d = std::max(ddx * texture[0].width(), ddy * texture[0].height());
				mipmap_level = std::log2(d);

				// �������õ���mipmap�ȼ�С��0����ô����͵ȼ����в���
				if (mipmap_level < 0)
				{
					mipmap_level = 0;
					// ʹ��˫���Բ�ֵ������ȡ������ɫ
					TGAColor var = bilinearInterpolate(texture[mipmap_level],
						texcoord.x * texture[mipmap_level].width(),
						texcoord.y * texture[mipmap_level].height());

					// ����RGB��ɫֵ
					return glm::vec3(var.bgra[2] / 255.0, var.bgra[1] / 255.0, var.bgra[0] / 255.0);
				}

				// �������õ���mipmap�ȼ��������ȼ�����ô����ߵȼ����в���
				if (mipmap_level > max_level)
				{
					mipmap_level = max_level;
					// ʹ��˫���Բ�ֵ������ȡ������ɫ
					TGAColor var = bilinearInterpolate(texture[mipmap_level],
						texcoord.x * texture[mipmap_level].width(),
						texcoord.y * texture[mipmap_level].height());

					// ����RGB��ɫֵ
					return glm::vec3(var.bgra[2] / 255.0, var.bgra[1] / 255.0, var.bgra[0] / 255.0);
				}
			}

			// �����mipmap�ȼ������½�
			int f_mipmap_level = floor(mipmap_level);
			int c_mipmap_level = ceil(mipmap_level);

			// ����½��Ƿ�С��0��������򴥷��ϵ�
			if (f_mipmap_level < 0) __debugbreak();

			// ����������mipmap�ȼ��Ͻ���˫���Բ�ֵ����ȡ������ɫֵ
			TGAColor f_var = bilinearInterpolate(texture[f_mipmap_level],
				texcoord.x * texture[f_mipmap_level].width(),
				texcoord.y * texture[f_mipmap_level].height());
			TGAColor c_var = bilinearInterpolate(texture[c_mipmap_level],
				texcoord.x * texture[c_mipmap_level].width(),
				texcoord.y * texture[c_mipmap_level].height());

			// ��������ɫֵ�������Բ�ֵ����ȡ������ɫֵ
			TGAColor var = ColorLerp(f_var, c_var, mipmap_level - f_mipmap_level);

			// ����RGB��ɫֵ
			return glm::vec3(var.bgra[2] / 255.0, var.bgra[1] / 255.0, var.bgra[0] / 255.0);
		}
		// �����������Ϊ�գ��򷵻��������ɫֵ��ʾ����
		else 
		{
			return glm::vec3(-200, -200, -200);
		}
	}

	void calculate_Texture(const Material* material, const glm::vec2 texcoord,
		glm::vec3& Ka, glm::vec3& Kd, glm::vec3& Ks, glm::vec3& Ke)		
	{
		// ��ͼ��Ke������ͼ���в�����ɫ
		Ke = sample(texcoord, material->map_Ke);
		// �������ʧ�ܣ���ôֱ��ʹ�ò��ʵ�Ke����
		if (Ke.x == -200 && Ke.y == -200 && Ke.z == -200)
			Ke = material->Ke;

		// ��ͼ��Kd������ͼ���в�����ɫ
		Kd = sample(texcoord, material->map_Kd);
		// �������ʧ�ܣ���ôֱ��ʹ�ò��ʵ�Kd����
		if (Kd.x == -200 && Kd.y == -200 && Kd.z == -200)
			Kd = material->Kd;

		// ��ͼ��Ka������ͼ���в�����ɫ
		Ka = sample(texcoord, material->map_Ka);
		// �������ʧ�ܣ���ôֱ��ʹ�ò��ʵ�Ka����
		if (Ka.x == -200 && Ka.y == -200 && Ka.z == -200)
			Ka = material->Ke;

		// ��ͼ��Ks������ͼ���в�����ɫ
		Ks = sample(texcoord, material->map_Ks);
		// �������ʧ�ܣ���ôֱ��ʹ�ò��ʵ�Ks����
		if (Ks.x == -200 && Ks.y == -200 && Ks.z == -200)
			Ks = material->Ke;
	}

	virtual void VertexShader(Triangle& primitive)
	{
		modeling = Model_Matrix(glm::vec3(0, 0, 0), angle, glm::vec3(0, 1, 0));
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
		glm::vec3 Ka;
		glm::vec3 Kd;
		glm::vec3 Ks;
		glm::vec3 Ke;
		glm::vec3 Normal;

		calculate_Texture(material, vertex.texcoord, Ka, Kd, Ks, Ke);

		Normal = glm::normalize(glm::vec3(vertex.normal));

		//Bump
		if (!material->map_Bump.empty())
		{
			//�����߿ռ䷨����ͼ�еõ�����
			glm::vec3 uv = sample(vertex.texcoord, material->map_Bump);

			glm::vec3 uv_vec(uv.x * 2 - 1, uv.y * 2 - 1, uv.z * 2 - 1);
			uv_vec = glm::normalize(uv_vec);

			glm::vec3 p1p0 = glm::vec3(tri_cameraspace.vertex[1].vertex - tri_cameraspace.vertex[0].vertex);
			glm::vec3 p2p0 = glm::vec3(tri_cameraspace.vertex[2].vertex - tri_cameraspace.vertex[0].vertex);
			glm::vec3 fu = glm::vec3(tri_cameraspace.vertex[1].texcoord.x - tri_cameraspace.vertex[0].texcoord.x,
				tri_cameraspace.vertex[2].texcoord.x - tri_cameraspace.vertex[0].texcoord.x, 0);
			glm::vec3 fv = glm::vec3(tri_cameraspace.vertex[1].texcoord.y - tri_cameraspace.vertex[0].texcoord.y,
				tri_cameraspace.vertex[2].texcoord.y - tri_cameraspace.vertex[0].texcoord.y, 0);

			glm::mat3 A = glm::transpose(glm::mat3(p1p0, p2p0, Normal));
			glm::mat3 A_inverse = glm::inverse(A);

			glm::vec3 T = A_inverse * fu;;
			glm::vec3 B = A_inverse * fv;

			glm::mat3 TBN(glm::normalize(T), glm::normalize(B), Normal);
			Normal = TBN * uv_vec;
		}

		float Ie = 0;
		float Ns = material->Ns;
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
			auto irradiance = glm::dot(Normal, lightdir);
			auto diffuse = Kd * std::max(0.0f, irradiance) * light.intensity / r_2;

			//specular
			auto viewdir = glm::normalize(-pos);
			auto h = glm::normalize(viewdir + lightdir);
			auto specular = Ks * light.intensity * std::pow(std::max(0.0f, glm::dot(h, glm::vec3(vertex.normal))), Ns);
			//+specular
			color += emission + ambient + diffuse ;
		}
		color = color * 255.0f;

		color.x = color.x > 255.0f ? 255.0f : color.x;
		color.y = color.y > 255.0f ? 255.0f : color.y;
		color.z = color.z > 255.0f ? 255.0f : color.z;

		//if (color.x == 0 && color.y == 0 && color.z == 0)
		   // __debugbreak();

		return TGAColor(color.x, color.y, color.z);
	}

	virtual void setmtl(Material& _material)
	{
		material = &_material;
	}

	virtual void setddx(const float _ddx) override
	{
		ddx = _ddx;
	}

	virtual void setddy(const float _ddy) override
	{
		ddy = _ddy;
	}

public:
	float theta_uniform;
	std::vector<PointLight> lights;
private:
	Material* material;
	Vertex vertex;
	float ddx = 1;
	float ddy = 1;
	Triangle tri_cameraspace;
	glm::mat4 modeling, viewing, projection;
};


void main()
{
	uint16_t frame = 0;
    auto begin = std::chrono::steady_clock::now();

    //std::vector<Triangle> TriangleList;

    std::vector<std::shared_ptr<Mesh>> MeshList;
    //Model model("obj/Ranni/", "Ranni.obj");
	//Model model("obj/Marry/", "Marry.obj");
    Model model("obj/diablo3_pose/", "diablo3_pose.obj");
    for (auto& o : model.objects)
    {
        for (auto& m : o.meshes)
        {
            std::shared_ptr<Mesh> m1 = std::make_shared<Mesh>(m);
            MeshList.push_back(m1);
        }
    }

	std::vector<std::shared_ptr<Triangle>> TriangleList;

	{
		glm::vec3 v[3];
		v[0] = glm::vec3(2, 0, -2);
		v[1] = glm::vec3(0, 2, -2);
		v[2] = glm::vec3(-2, 0, -2);
		std::shared_ptr<Triangle> t1(new Triangle(v));
		t1->setColor(0, TGAColor(255, 0, 0));
		t1->setColor(1, TGAColor(0, 255, 0));
		t1->setColor(2, TGAColor(0, 0, 255));
		t1->setNormal(0, glm::vec4(0, 0, 1, 0));
		t1->setNormal(1, glm::vec4(0, 0, 1, 0));
		t1->setNormal(2, glm::vec4(0, 0, 1, 0));
		TriangleList.push_back(t1);
	}

	Shader shader;
	PointLight light = PointLight(glm::vec3(20, 20, 20), 1500);
	light.position = View_Matrix(eye_pos, gaze_dir, view_up) * glm::vec4(light.position, 0);
		
	shader.lights.push_back(light);

	while (angle < 360)
	{
		//eye_pos=SetCamera(eye_pos, angle, glm::vec3(0, 1, 0));
	
		std::string filename = "result\\output" + std::to_string(frame);

		TGAImage image(800, 600, TGAImage::RGB);
		//SetCamera(eye_pos, angle, glm::vec3(0, 1, 0));
		Rasterizer rast(filename, image);
        rast.TurnOnBackCulling();
		rast.draw(MeshList, shader);
		rast.output();

        std::cout << "frame:" << frame << std::endl;
        frame++;
        angle += 5;
	}

}


