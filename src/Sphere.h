#pragma once

#include <glm/glm.hpp>
#include <string>
#include "SceneObject.h"
#include "Ray3D.h"
#include "HitResult.h"

class Sphere : public SceneObject {
public:
	// Call parent constructor to create transform matrix and apply material
	Sphere(std::string _name, Transform _transf, Material _mat) : SceneObject(_name, _transf, _mat) {};

	bool IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) override;
	glm::dvec4 GetRandomPointOnSurface() override;

private:
	// Checks 2 numbers against a given range, places the smaller # in the range into result, or returns false if neither are in the range
	bool SelectSmallestInRange(double a, double b, double min, double max, double& result);
};