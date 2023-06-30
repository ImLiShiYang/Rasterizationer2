#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tgaimage.h"

class Material
{
public:
	Material() = default;
	Material(const std::string& filename, const std::string& root, const std::string& materialName);

public:
	std::string name;

	glm::vec3 Ke;
	glm::vec3 Kd;
	glm::vec3 Ka;
	glm::vec3 Ks;
	float illum;
	float Ns;
	float d;
	float Ni;

	std::vector<TGAImage> map_Ke;
	std::vector<TGAImage> map_Kd;
	std::vector<TGAImage> map_Ka;
	std::vector<TGAImage> map_Ks;
	std::vector<TGAImage> map_Ns;
	std::vector<TGAImage> map_d;

	glm::vec3 Bump;
	std::vector<TGAImage> map_Bump;
};


inline int calculate_mipmaplevels(int mipmap_width, int mipmap_height)
{
	int mipmaplevel = 1;
	int size = std::min(mipmap_width, mipmap_height);
	while (size > 1)
	{
		//缩放的次数就是层级数
		mipmaplevel++;
		//每次缩放一半
		size = std::max(1, size / 2);
	}
	return mipmaplevel;
}

inline std::vector<TGAImage> generateMipmap(const TGAImage& texture)
{
	std::vector<TGAImage> mipmap;

	int mipmap_width = texture.width();
	int mipmap_height = texture.height();

	int mipmap_levels = calculate_mipmaplevels(mipmap_width, mipmap_height);

	for (int level = 0; level < mipmap_levels; level++)
	{
		TGAImage mipmap_level_data(mipmap_width, mipmap_height, TGAImage::RGB);
		for (int y = 0; y < mipmap_height; y++)
		{
			for (int x = 0; x < mipmap_width; x++)
			{
				int image_x = x * texture.width() / mipmap_width;
				int image_y = y * texture.height() / mipmap_height;
				TGAColor pixel = texture.get(image_x, image_y);

				mipmap_level_data.set(x, y, pixel);
			}
		}

		mipmap.push_back(mipmap_level_data);

		mipmap_width = std::max(1, mipmap_width / 2);
		mipmap_height = std::max(1, mipmap_height / 2);
	}
	return mipmap;
}

#endif // !MATERIAL_H



