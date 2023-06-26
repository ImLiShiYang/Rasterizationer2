#ifndef SHADER_H
#define SHADER_H

#include "Triangle.h"
#include "light.h"

class FragmentShaderPayload
{
public:
	FragmentShaderPayload() = default;
	FragmentShaderPayload(const Vertex& v,const PointLight l):vertex(v), light(l){}

public:
	Vertex vertex;
	PointLight light;
};

class IShader {
public:
	//virtual void setmtl(Material& material) = 0;
	//virtual void setddx(const float ddx) = 0;
	//virtual void setddy(const float ddy) = 0;
	virtual void VertexShader(Triangle& primitive) = 0;
	virtual TGAColor FragmentShader(Vertex& vertex) = 0;
};



static Vertex BlinnPhoneShader(const FragmentShaderPayload& payload)
{
	//自发光
	glm::vec3 ke(0, 0, 0);
	float ie = 0;
	glm::vec3 emission = ie * ke;
	//环境光
	glm::vec3 ka(0.005f);
	glm::vec3 ambient = ka * 50.f;
	//漫反射光
	glm::vec3 kd(payload.vertex.vertexColor.bgra[2], payload.vertex.vertexColor.bgra[1], payload.vertex.vertexColor.bgra[0]);
	glm::vec3 pos = payload.vertex.vertex;
	glm::vec3 normal = glm::normalize(glm::vec3(payload.vertex.normal));
	glm::vec3 light_dir = glm::normalize(payload.light.position - pos);
	float r_r = glm::distance(payload.light.position, pos) * glm::distance(payload.light.position, pos);
	glm::vec3 diffuse = kd * std::max(0.0f, glm::dot(light_dir, normal)) * payload.light.intensity / r_r;
	//高光
	glm::vec3 ks(0.7937);
	glm::vec3 view_dir = glm::normalize( - pos);
	glm::vec3 half_vector = glm::normalize(view_dir + light_dir);
	glm::vec3 specular = ks * std::pow(std::max(0.0f, glm::dot(half_vector, normal)), 20.f) * payload.light.intensity / r_r;
	//
	auto color = emission + ambient + diffuse + specular;

	color = color * 255.0f;

	color.x = color.x > 255.0f ? 255.0f : color.x;
	color.y = color.y > 255.0f ? 255.0f : color.y;
	color.z = color.z > 255.0f ? 255.0f : color.z;

	Vertex v = payload.vertex;
	v.vertexColor = TGAColor(color.x, color.y, color.z);
	return v;
}


#endif // !SHADER_H


