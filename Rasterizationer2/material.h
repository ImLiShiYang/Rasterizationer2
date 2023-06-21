#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

class Material
{
public:
	Material() = default;
	Material(const std::string& filename, const std::string& root, const std::string& materialName);
};


#endif // !MATERIAL_H



