#pragma once
#include "SceneObject.h"

using namespace std;
using namespace glm;

SceneObject::SceneObject(std::string _name, Transform _transf, Material _mat) {
	// Since all objects in this project are static and independent, the MatrixStack class is not required
	// (We can just calculate the transformations once, no need for hierarchies or dynamic transf calculations)
	
	// The transformation matrix to convert this object from local->world space
	glm::dmat4 modelMtx = glm::dmat4(1.0f);
	modelMtx *= translate(glm::dmat4(1.0f), dvec3(_transf.loc));
	modelMtx *= eulerAngleXYZ(_transf.rot.x, _transf.rot.y, _transf.rot.z);
	modelMtx *= scale(glm::dmat4(1.0f), _transf.scale);

	// For this implementation, you  don't actually need the model matrix. We convert rays from world space -> object space,
	// but we never have to convert any locations from object->world since it uses the t value to find world hit locations
	invMtx = inverse(modelMtx);
	// Still need the inverse transpose to convert normals from object->world, though
	invTranspMtx = transpose(invMtx);
	mat = _mat;
	name = _name;
}

bool SceneObject::Hit(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Apply transformations to the ray to change it to local space
	Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);

	// Check intersection (NOTE: localRay direction is NOT normalized)
	// Since I didn't normalize the ray direction vector, the t distance stored in outHit is in world space
	return IntersectLocal(localRay, outHit, tMin, tMax);
}

bool SceneObject::SelectSmallestInRange(double a, double b, double min, double max, double& result) {
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
