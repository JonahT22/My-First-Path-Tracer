#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "HitResult.h"
#include "Ray3D.h"
#include "Light.h"

struct Material {
	glm::dvec3 kd;
	glm::dvec3 ks;
	glm::dvec3 ke;
	// Probability that rays hitting this object will use a reflective BRDF
	// 1 = All rays are reflected, 0 = All rays use a diffuse/BlinnPhong BRDF
	double reflectance;
	// Amount of randomness to apply to reflected rays. 0 = uniform diffuse, 1 = perfect reflection
	double roughness = 0;
	// 'width/strength' of specular highlights
	double specularExp;

	Material() :
		kd(glm::dvec3(1.0, 1.0, 1.0)),
		ks(glm::dvec3(1.0, 1.0, 1.0)),
		ke(glm::dvec3(0, 0, 0)),
		reflectance(0.0),
		specularExp(100.0)
	{}
	Material(glm::dvec3 _kd, glm::dvec3 _ks, glm::dvec3 _ke, double _reflectance, double _specularExp) :
		kd(_kd),
		ks(_ks),
		ke(_ke),
		reflectance(_reflectance),
		specularExp(_specularExp)
	{}

	glm::dvec3 ShadeBlinnPhong(const Ray3D& ray, const HitResult& hit, const std::shared_ptr<Light> light) const {
		// Diffuse component
		glm::dvec4 lightVec = glm::normalize(light->GetLocation() - hit.loc);
		glm::dvec3 cd = kd * std::max(0.0, glm::dot(lightVec, hit.nor));

		// Specular component
		glm::dvec4 eyeVec = -1.0 * ray.dir;
		glm::dvec4 halfVec = glm::normalize(eyeVec + lightVec); // Since it's normalized, it doesn't matter that it's not / 2
		glm::dvec3 cs = ks * std::pow(std::max(0.0, glm::dot(halfVec, hit.nor)), specularExp);

		return light->SampleLight(hit.loc) * (cd + cs);
	}
	
	glm::dvec3 ShadeDiffuse(const Ray3D& ray, const HitResult& hit, const std::shared_ptr<Light> light) const {
		glm::dvec4 lightVec = glm::normalize(light->GetLocation() - hit.loc);
		glm::dvec3 cd = kd * std::max(0.0, glm::dot(lightVec, hit.nor));

		return light->SampleLight(hit.loc) * cd;
	}
};