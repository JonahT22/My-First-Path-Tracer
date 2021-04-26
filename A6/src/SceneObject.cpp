#pragma once
#include "SceneObject.h"

using namespace std;
using namespace glm;

SceneObject::SceneObject(Transform _transf, Material _mat) {
	// Since all objects in this project are static and independent, the MatrixStack class is not required
	// (We can just calculate the transformations once, no need for hierarchies or dynamic transf calculations)
	transform = _transf;
	transMtx *= translate(glm::dmat4(1.0f), dvec3(_transf.loc));
	transMtx *= eulerAngleXYZ(_transf.rot.x, _transf.rot.y, _transf.rot.z);
	transMtx *= scale(glm::dmat4(1.0f), _transf.scale);

	mat = _mat;
}

bool SceneObject::Hit(Ray3D ray, HitResult& outHit, double tMin, double tMax) {
	// Apply transformations to the ray to change it to local space
	dmat4 invMtx = inverse(transMtx);
	Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);

	// Check intersection (NOTE: localRay direction is NOT normalized)
	// Since I didn't normalize the ray direction vector, the t distance stored in outHit is in world space
	return IntersectLocal(localRay, outHit, tMin, tMax);
}

glm::dmat4 SceneObject::GetInverseTranspose()
{
	return transpose(inverse(transMtx));
}

bool SceneObject::SelectSmallestInRange(double a, double b, double min, double max, double& result)
{
	bool aValid = (a > min && a < max);
	bool bValid = (b > min && b < max);
	if (aValid || bValid) {
		// If both are valid, choose the smaller one
		if (aValid && bValid) {
			result = (a < b) ? a : b;
		}
		// If exec makes it here, ONLY one is valid
		else if (aValid) result = a;
		else result = b;

		return true;
	}

	return false;
}
