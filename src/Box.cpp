#pragma once
#include "Box.h"

using namespace std;
using namespace glm;

bool Box::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Optimized ray-box intersection adapted from http://people.csail.mit.edu/amy/papers/box-jgt.pdf

	// Assume box is at the origin, axis-aligned, with bounds from -0.5 to 0.5 in x, y, and z
	// tvals[0] corresponds to the closer hit, tvals[1] is the farther one
	double tvals[2];
	// The normals of each the side of the box that each tval corresponds to
	dvec4 normals[2];

	// Start with the t values & normals from x-axis intersections
	// Flip sign of the negative/positive bounds when the ray direction is negative (since far/near sides will be swapped)
	tvals[0] = ((-0.5 * ray.sign[0]) - ray.start.x) * ray.invDir.x;
	tvals[1] = (( 0.5 * ray.sign[0]) - ray.start.x) * ray.invDir.x;
	normals[0] = (double)ray.sign[0] * dvec4(-1, 0, 0, 0);
	normals[1] = (double)ray.sign[0] * dvec4( 1, 0, 0, 0);

	// Find min/max t in y direction
	double tymin = ((-0.5 * ray.sign[1]) - ray.start.y) * ray.invDir.y;
	double tymax = (( 0.5 * ray.sign[1]) - ray.start.y) * ray.invDir.y;

	// If new y min/max are completely outside the current t min/max, return false immediately
	if ((tvals[0] > tymax) || (tymin > tvals[1])) {
		return false;
	}
	// Check if new min/max values are closer to the center of the box
	if (tymin > tvals[0]) {
		tvals[0] = tymin;
		// If a new tval is found, update the corresponding normal as well
		normals[0] = (double)ray.sign[1] * dvec4(0, -1, 0, 0);
	}
	if (tymax < tvals[1]) {
		tvals[1] = tymax;
		normals[1] = (double)ray.sign[1] * dvec4(0, 1, 0, 0);
	}

	// Repeat the same process as above, but with the z direction
	double tzmin = ((-0.5 * ray.sign[2]) - ray.start.z) * ray.invDir.z;
	double tzmax = (( 0.5 * ray.sign[2]) - ray.start.z) * ray.invDir.z;
	if ((tvals[0] > tzmax) || (tzmin > tvals[1])) {
		return false;
	}
	if (tzmin > tvals[0]) {
		tvals[0] = tzmin;
		normals[0] = (double)ray.sign[2] * dvec4(0, 0, -1, 0);
	}
	if (tzmax < tvals[1]) {
		tvals[1] = tzmax;
		normals[1] = (double)ray.sign[2] * dvec4(0, 0, 1, 0);
	}

	// Choose the smallest t value within the valid range, and choose the corresponding normal
	int chosenIdx = SelectSmallestInRange(tvals, tMin, tMax);
	if (chosenIdx != -1) {
		// Try updating the HitResult with whichever t value was the smallest in the range
		if (outHit.UpdateTMin(tvals[chosenIdx])) {
			// If the HitResult ended up finding a new minT, update the normal value in the hit result and return true
			outHit.nor = normals[chosenIdx];
			return true;
		}
	}

	// return false if:
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated w/a new value)
	return false;
}

glm::dvec4 Box::GetRandomPointOnSurface()
{
	return transf.translation;
}

bool Box::IsInUnitSquare(const glm::vec4& v) const {
	return (v.x > -0.5 && v.x < 0.5) && (v.z > -0.5 && v.z < 0.5);
}
