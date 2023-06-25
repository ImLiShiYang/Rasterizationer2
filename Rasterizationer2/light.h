#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Light
{
public:
	virtual ~Light() = default;
	virtual std::string getType() = 0;
};

class PointLight : public Light
{
public:
    glm::vec3 position;
    float intensity;
public:
    PointLight(const glm::vec3& position, const float intensity)
        :position(position), intensity(intensity) {}
    std::string getType() { return std::string("PointLight"); }
};

class DirectionalLight : public Light
{
public:
    glm::vec3 direction;
    float intensity;
public:
    DirectionalLight(const glm::vec3& direction, const float intensity)
        :direction(direction), intensity(intensity) {}
    std::string getType() { return std::string("DirectionalLight"); }
};

#endif // !LIGHT_H


