#pragma once
#include "Box.h"

using namespace std;
using namespace glm;

bool Box::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Optimized ray-box intersection adapted from http://people.csail.mit.edu/amy/papers/box-jgt.pdf

	// Assume box is at the origin, axis-aligned, with bounds from -0.5 to 0.5 in x, y, and z
	// Bounds are the same in the x, y, and z directions
	double bounds[2] = {-0.5, 0.5};

	// Start with the t values from x-axis intersections
	// Swap the negative/positive bounds when the ray direction is negative (since far/near sides will be opposite)
	double t1 = (bounds[ray.sign[0]] - ray.start.x) * ray.invDir.x;
	double t2 = (bounds[1 - ray.sign[0]] - ray.start.x) * ray.invDir.x;

	// Find min/max t in y direction
	double tymin = (bounds[ray.sign[1]] - ray.start.y) * ray.invDir.y;
	double tymax = (bounds[1 - ray.sign[1]] - ray.start.y) * ray.invDir.y;

	// If new y min/max are completely outside the current t min/max, return false immediately
	if ((t1 > tymax) || (tymin > t2)) {
		return false;
	}
	// Check if new min/max values are closer to the center of the box
	if (tymin > t1) t1 = tymin;
	if (tymax < t2) t2 = tymax;

	// Repeat the same process as above, but with the z direction
	double tzmin = (bounds[ray.sign[2]] - ray.start.z) * ray.invDir.z;
	double tzmax = (bounds[1 - ray.sign[2]] - ray.start.z) * ray.invDir.z;
	if ((t1 > tzmax) || (tzmin > t2)) {
		return false;
	}
	if (tzmin > t1) t1 = tzmin;
	if (tzmax < t2) t2 = tzmax;

	// Choose the smallest t value within the valid range, and check if it's smaller than the value
	// that's already in outHit. If so, return true
	double timeToUpdate = std::numeric_limits<double>::max();
	if (SelectSmallestInRange(t1, t2, tMin, tMax, timeToUpdate)) {
		// Try updating the HitResult with whichever t value was the smallest in the range
		if (outHit.UpdateTMin(timeToUpdate)) {
			// If the HitResult ended up finding a new minT, update the normal value in the hit result and return true
			outHit.nor = dvec4(0, 1, 0, 0);
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
