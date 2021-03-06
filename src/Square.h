#pragma once

#include <glm/glm.hpp>
#include <string>
#include <cstdlib>
#include <memory>
#include "SceneObject.h"
#include "Ray3D.h"
#include "HitResult.h"

class Square : public SceneObject {
public:
	// Call parent constructor to create transform matrix and apply material
	Square(std::string _name, Transform _transf, std::shared_ptr<Material> _mat) : SceneObject(_name, _transf, _mat) {
		hasRandomPointMethodDefined = true;
	};

	bool IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) override;
	glm::dvec4 GetRandomPointOnSurface(double& pdf, glm::dvec4& normal) override;

private:
	// Check if the given value is between -0.5 and 0.5
	bool IsInUnitSquare(const glm::vec4& v) const;
};