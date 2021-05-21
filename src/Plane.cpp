#pragma once
#include "Plane.h"

using namespace std;
using namespace glm;

bool Plane::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Just like the sphere, the ray will be transformed into the local space of this plane, so
	// assume plane is at the origin, parallel with x-z plane, with no transformations applied 

	// Full equation: t = dot(n, (planeLoc - ray.start)) / dot(n, ray.dir)
	// Since we're in local space, assume n = <0, 1, 0> and planeLoc = <0, 0, 0>
	double numerator = (-1.0 * ray.start).y;
	double denom = ray.dir.y;

	// Rare, but make sure we don't have a divide-by-zero before calculating t
	if (denom != 0) {
		double newT = numerator / denom;
		if (newT > tMin && newT < tMax) {
			// Try updating the HitResult with whichever t value was the smallest in the range
			if (outHit.UpdateTMin(newT)) {
				// If the HitResult ended up finding a new minT, update the normal value in the hit result and return true
				outHit.nor = dvec4(0, 1, 0, 0);
				return true;
			}
		}
	}

	// return false if:
	// - Denominator is zero (Ray is parallel to plane)
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated w/a new value)
	return false;
}

glm::dvec4 Plane::GetRandomPointOnSurface(double& pdf, dvec4& normal)
{
	return transf.translation;
}
