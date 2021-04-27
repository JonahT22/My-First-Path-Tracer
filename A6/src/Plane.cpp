#pragma once
#include "Plane.h"

using namespace std;
using namespace glm;

bool Plane::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Just like the sphere, the ray will be transformed into the local space of this plane, so
	// assume plane is at the origin, parallel with x-z plane, with no transformations applied 
	dvec4 n(0, 1, 0, 0); // normal
	dvec4 c(0, 0, 0, 1); // location

	double numerator = dot(n, (c - ray.start));
	double denom = dot(n, ray.dir);

	if (denom != 0) {
		// Choose the smallest t value within the valid range, and check if it's smaller than the value
		// that's already in outHit. If so, return true
		double newT = numerator / denom;
		if (newT > tMin && newT < tMax) {
			// Try updating the hit result with whichever t value was the smallest in the range
			if (outHit.UpdateTMin(newT)) {
				// If it ended up finding a new minT, update the normal value in the hit result
				outHit.nor = n; // n is already unit length
				return true;
			}
		}
	}

	// return false if:
	// - Denominator is zero (Ray is parallel to plane)
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated)
	return false;
}