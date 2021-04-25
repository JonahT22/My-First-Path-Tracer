#pragma once

#include <glm/glm.hpp>
#include "HitResult.h"
#include "Ray3D.h"
#include "PointLight.h"

struct Material {
	glm::vec3 kd;
	glm::vec3 ks;
	glm::vec3 ka;
	float exp;

	Material() :
		kd(glm::vec3(1.0, 1.0, 1.0)),
		ks(glm::vec3(1.0, 1.0, 1.0)),
		ka(glm::vec3(0.1, 0.1, 0.1)),
		exp(100.0)
	{}
	Material(glm::vec3 _kd, glm::vec3 _ks, glm::vec3 _ka, float _exp) :
		kd(_kd),
		ks(_ks),
		ka(_ka),
		exp(_exp)
	{}

	// TODO: make a material.cpp
	glm::vec3 ShadeBlinnPhong(HitResult& hit, Ray3D& ray, PointLight& light) const {
		// Note: don't handle the ambient component here
		// Diffuse component
		glm::vec4 lightVec = glm::normalize(light.pos - hit.loc);
		glm::vec3 cd = kd * std::max(0.0f, glm::dot(lightVec, hit.nor));

		// Specular component
		// TODO: is eyeVec the same as ray.dir * -1?
		glm::vec4 eyeVec = glm::normalize(ray.start - hit.loc);
		//glm::vec4 eyeVec = -1.0f * ray.dir;
		glm::vec4 halfVec = glm::normalize((eyeVec + lightVec) / 2.0f);
		glm::vec3 cs = ks * std::pow(std::max(0.0f, glm::dot(halfVec, hit.nor)), exp);

		return light.intensity * (cd + cs);
	}
};