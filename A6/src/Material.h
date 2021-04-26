#pragma once

#include <glm/glm.hpp>
#include "HitResult.h"
#include "Ray3D.h"
#include "PointLight.h"

struct Material {
	glm::dvec3 kd;
	glm::dvec3 ks;
	glm::dvec3 ka;
	// 0-1 value, where 0 = perfectly reflective, 1 = perfectly diffuse
	double roughness;
	// 'width/strength' of specular highlights
	double specularExp;

	Material() :
		kd(glm::dvec3(1.0, 1.0, 1.0)),
		ks(glm::dvec3(1.0, 1.0, 1.0)),
		ka(glm::dvec3(0.1, 0.1, 0.1)),
		roughness(1.0),
		specularExp(100.0)
	{}
	Material(glm::dvec3 _kd, glm::dvec3 _ks, glm::dvec3 _ka, double _roughness, double _specularExp) :
		kd(_kd),
		ks(_ks),
		ka(_ka),
		roughness(_roughness),
		specularExp(_specularExp)
	{}

	glm::dvec3 ShadeBlinnPhong(Ray3D& ray, HitResult& hit, PointLight& light) const {
		// Note: don't handle the ambient component here
		// Diffuse component
		glm::dvec4 lightVec = glm::normalize(light.pos - hit.loc);
		glm::dvec3 cd = kd * std::max(0.0, glm::dot(lightVec, hit.nor));

		// Specular component
		glm::dvec4 eyeVec = -1.0 * ray.dir;
		glm::dvec4 halfVec = glm::normalize(eyeVec + lightVec); // Since it's normalized, it doesn't matter that it's not / 2
		glm::dvec3 cs = ks * std::pow(std::max(0.0, glm::dot(halfVec, hit.nor)), specularExp);

		return light.intensity * (cd + cs);
	}
};