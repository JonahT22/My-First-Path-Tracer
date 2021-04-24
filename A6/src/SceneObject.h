#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Ray3D.h"
#include "Transform.h"
#include "HitResult.h"

class SceneObject {
public:
	SceneObject() = default;

	// Calcualte the transformation matrix for this object
	void CreateTransformMtx(Transform _transf);

	// By default, hits go from 0 to inf unless override is specified
	bool Hit(Ray3D ray, HitResult& outHit, double tMin = 0, double tMax = std::numeric_limits<double>::max());
	glm::mat4 GetInverseTranspose();

	// Check intersection in local space, return true if found an intersection that is closer than the hit's t value
	virtual bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) = 0;

protected:
	bool SelectSmallestInRange(double a, double b, double min, double max, double& result);

	// The transformation matrix to convert this object from local->world space
	glm::mat4 transMtx = glm::mat4(1.0f);
	// TODO: is this variable necessary after finding transMtx?
	Transform transform;
};