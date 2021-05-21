#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "SceneObject.h"
#include "Ray3D.h"
#include "HitResult.h"

class Sphere : public SceneObject {
public:
	// Call parent constructor to create transform matrix and apply material
	Sphere(std::string _name, Transform _transf, std::shared_ptr<Material> _mat) : SceneObject(_name, _transf, _mat) {};

	bool IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) override;
	glm::dvec4 GetRandomPointOnSurface(double& pdf, glm::dvec4& normal) override;
};