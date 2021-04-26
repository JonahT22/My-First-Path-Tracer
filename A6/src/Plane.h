#pragma once

#include <glm/glm.hpp>
#include "SceneObject.h"
#include "Ray3D.h"
#include "HitResult.h"

class Plane : public SceneObject {
public:
	// Call parent constructor to create transform matrix and apply material
	Plane(Transform _transf, Material _mat) : SceneObject(_transf, _mat) {};

	bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) override;
};