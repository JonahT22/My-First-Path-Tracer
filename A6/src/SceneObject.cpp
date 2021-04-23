#pragma once
#include "SceneObject.h"

using namespace glm;

void SceneObject::CreateTransformMtx(Transform _transf) {
	// Since all objects in this project are static and independent, the MatrixStack class is not required
		// (We can just calculate the transformations once, no need for hierarchies or dynamic transf calculations)
	transform = _transf;
	transMtx *= translate(glm::mat4(1.0f), vec3(_transf.loc));
	transMtx *= eulerAngleXYZ(_transf.rot.x, _transf.rot.y, _transf.rot.z);
	transMtx *= scale(glm::mat4(1.0f), _transf.scale);
}

bool SceneObject::Hit(Ray3D ray, HitResult& outHit) {
	// Apply transformations to the ray to change it to local space
	mat4 invMtx = inverse(transMtx);
	Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);
	// Check intersection (NOTE: ray direction is NOT normalized)
	if (IntersectLocal(localRay, outHit)) {
		// TODO: convert the outHit back to world coords
		// WARNING: if the IntersectLocal function didn't change the value of outHit,
		// then I shouldn't try to convert its values here. Maybe only return true if the 
		// raycast actually updated outHit? 
		return true;
	}
	else return false;
}
