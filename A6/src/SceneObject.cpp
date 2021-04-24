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

bool SceneObject::Hit(Ray3D ray, HitResult& outHit, double tMin, double tMax) {
	// Apply transformations to the ray to change it to local space
	mat4 invMtx = inverse(transMtx);
	Ray3D localRay(invMtx * ray.start, invMtx * ray.dir);
	// Check intersection (NOTE: ray direction is NOT normalized)
	if (IntersectLocal(localRay, outHit, tMin, tMax)) {
		// Since I didn't normalize the ray direction vector, my t distance is already in world space
		// NOTE: this only gets executed if the IntersectLocal function actually updated the values in outHit
		
		// New position from tMin
		outHit.loc = ray.FindLocAtTime(outHit.tMin); 
		// Use Inverse transpose to convert nor to world space
		outHit.nor = normalize(transpose(invMtx) * outHit.nor);

		// Color with normals
		// Normal -> color conversion taken from assigment handout
		unsigned char r = 255 * (0.5 * outHit.nor.x + 0.5);
		unsigned char g = 255 * (0.5 * outHit.nor.y + 0.5);
		unsigned char b = 255 * (0.5 * outHit.nor.z + 0.5);
		outHit.color = vec3(r, g, b);
		
		return true;
	}
	else return false;
}

// Checks 2 numbers against a given range, places the smaller # in the range into result, or returns false if neither are in the range
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
