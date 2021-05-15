#pragma once
#include "SceneObject.h"

using namespace std;
using namespace glm;

SceneObject::SceneObject(std::string _name, Transform _transf, Material _mat) {
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
