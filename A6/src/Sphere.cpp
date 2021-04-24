#pragma once
#include "Sphere.h"

using namespace glm;

bool Sphere::IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) {
	// Assume sphere is at origin with radius 1
	double a = dot(vec3(ray.dir), vec3(ray.dir));
	double b = 2.0 * dot(vec3(ray.dir), vec3(ray.start));
	double c = dot(vec3(ray.start), vec3(ray.start)) - 1.0;
	// Discriminant^2
	double d2 = pow(b, 2) - (4 * a * c);

	if (d2 >= 0) {
		// 1 or 2 solutions exist
		double t1 = (-1.0 * b + sqrt(d2)) / (2.0 * a);
		double t2 = (-1.0 * b - sqrt(d2)) / (2.0 * a);
		
		// Choose the smallest t value within the valid range, and check if it's smaller than the value
		// that's already in outHit. If so, return true
		double timeToUpdate = std::numeric_limits<double>::max();
		if (SelectSmallestInRange(t1, t2, tMin, tMax, timeToUpdate)) {
			// Try updating the hit result with whichever t value was the smallest in the range
			if (outHit.UpdateTMin(timeToUpdate)) {
				// If it ended up finding a new minT, update the normal value in the hit result
				vec4 localPos = ray.FindLocAtTime(outHit.t);
				outHit.nor = normalize(localPos);
				outHit.nor.w = 0.0; // outHit.nor is a vector, so the w value is always 0
				return true;
			}
		}
	}

	// return false if:
	// - discriminant was negative
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated)
	return false;
}