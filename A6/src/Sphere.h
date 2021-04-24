#pragma once

#include <glm/glm.hpp>
#include "SceneObject.h"
#include "HelperStructs.h"

class Sphere : public SceneObject {
public:
	Sphere() = default;

	bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) override;
};