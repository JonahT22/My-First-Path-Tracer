#pragma once
#include "SceneObject.h"

using namespace std;
using namespace glm;

SceneObject::SceneObject(std::string _name, Transform _transf, shared_ptr<Material> _mat) {
	// Since all objects in this project are static and independent, the MatrixStack class is not required
	// (We can just calculate the transformations once, no need for hierarchies or dynamic transf calculations)
	
	// The transformation matrix to convert this object from local->world space
	modelMtx = glm::dmat4(1.0f);
	modelMtx *= translate(glm::dmat4(1.0f), dvec3(_transf.translation));
	modelMtx *= eulerAngleXYZ(_transf.rotation.x, _transf.rotation.y, _transf.rotation.z);
	modelMtx *= scale(glm::dmat4(1.0f), _transf.scale);
	invMtx = inverse(modelMtx);
	invTranspMtx = transpose(invMtx);
	mat = _mat;
	name = _name;
	transf = _transf;
}

bool SceneObject::Hit(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Apply transformations to the ray to change it to local space
	Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);

	// Check intersection (NOTE: localRay direction is NOT normalized)
	// Since I didn't normalize the ray direction vector, the t distance stored in outHit is in world space
	return IntersectLocal(localRay, outHit, tMin, tMax);
}

int SceneObject::SelectSmallestInRange(double vals[2], double min, double max) {
	bool aValid = (vals[0] > min && vals[0] < max);
	bool bValid = (vals[1] > min && vals[1] < max);
	if (aValid || bValid) {
		// If both are valid, choose the smaller one
		int chosenIdx = 0;
		if (aValid && bValid) {
			chosenIdx = (vals[0] < vals[1]) ? 0 : 1;
		}
		// If exec makes it here, ONLY one is valid
		else if (aValid) chosenIdx = 0;
		else chosenIdx = 1;

		return chosenIdx;
	}

	return -1;
}
