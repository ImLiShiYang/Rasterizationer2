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

	TGAImage texture;

	std::vector<TGAImage> map_Ke;
	std::vector<TGAImage> map_Kd;
	std::vector<TGAImage> map_Ka;
	std::vector<TGAImage> map_Ks;
	std::vector<TGAImage> map_Ns;
	std::vector<TGAImage> map_d;

	glm::vec3 Bump;
	std::vector<TGAImage> map_Bump;
};


#endif // !MATERIAL_H



