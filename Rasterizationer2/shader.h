#ifndef SHADER_H
#define SHADER_H

#include "Triangle.h"
#include "light.h"
#include "material.h"

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
	virtual void setmtl(Material& material) = 0;
	//virtual void setddx(const float ddx) = 0;
	//virtual void setddy(const float ddy) = 0;
	virtual void VertexShader(Triangle& primitive) = 0;
	virtual TGAColor FragmentShader(Vertex& vertex) = 0;
};

//双线性插值，取点附近的四个点进行插值
inline TGAColor bilinearInterpolate(const TGAImage& texture, float u, float v)
{
	while (u > texture.width())
		u -= texture.width();
	while (u < 0)
		u += texture.width();
	while (v > texture.height())
		v -= texture.height();
	while (v < 0)
		v += texture.height();

	float uu = u - 0.5f;
	float vv = v - 0.5f;
	glm::vec2 leftTop(std::floor(uu), std::ceil(vv));
	glm::vec2 rightTop(std::ceil(uu), std::ceil(vv));

	TGAColor topColor = ColorLerp(texture.get(leftTop.x, leftTop.y), texture.get(rightTop.x, rightTop.y), uu - floor(uu));

	glm::vec2 leftBottom(std::floor(uu), std::floor(vv));
	glm::vec2 rightBottom(std::ceil(uu), std::floor(vv));

	TGAColor bottomColor = ColorLerp(texture.get(leftBottom.x, leftBottom.y), texture.get(rightBottom.x, rightBottom.y), uu - floor(uu));

	TGAColor color = ColorLerp(bottomColor, topColor, vv - floor(vv));
	return color; 
}

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


