#ifndef MODEL_H
#define MODEL_H

#include "material.h"
#include "Triangle.h"
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh
{
public:
	Mesh(const std::string& filename, const std::string& root, const std::string& materialname):
		material(filename, root, materialname) {}

public:
	Material material;
	std::vector<Triangle> primitives;

};

class Object
{
public:
	Object(){}

public:
	std::vector<Mesh> meshes;
};

class Model
{
public:
	Model(const std::string& root, const std::string& filename);
private:
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
public:
	std::vector<Object> objects;
};

#endif // !MODEL_H



