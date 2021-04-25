#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Ray3D.h"
#include "Transform.h"
#include "HitResult.h"
#include "Material.h"

class SceneObject {
public:
	// Assign material and calculate the transformation matrix
	SceneObject(Transform _transf, Material _mat);

	Material GetMaterial() { return mat; }

	// By default, hits go from 0 to inf unless override is specified
	bool Hit(Ray3D ray, HitResult& outHit, double tMin = 0, double tMax = std::numeric_limits<double>::max());
	glm::dmat4 GetInverseTranspose();

	// Check intersection in local space, return true if found an intersection that is closer than the hit's t value
	virtual bool IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) = 0;

protected:
	bool SelectSmallestInRange(double a, double b, double min, double max, double& result);
	// The transformation matrix to convert this object from local->world space
	glm::dmat4 transMtx = glm::dmat4(1.0f);
	// TODO: is this variable necessary after finding transMtx?
	Transform transform;
	Material mat;
};