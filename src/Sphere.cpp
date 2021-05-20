#pragma once
#include "Sphere.h"

using namespace std;
using namespace glm;

bool Sphere::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Assume sphere is at origin with radius 1
	double a = dot(dvec3(ray.dir), dvec3(ray.dir));
	double b = 2.0 * dot(dvec3(ray.dir), dvec3(ray.start));
	double c = dot(dvec3(ray.start), dvec3(ray.start)) - 1.0;
	// Discriminant^2
	double d2 = pow(b, 2) - (4 * a * c);

	if (d2 >= 0) {
		// 1 or 2 solutions exist
		double tvals[2];
		tvals[0] = (-1.0 * b + sqrt(d2)) / (2.0 * a);
		tvals[1] = (-1.0 * b - sqrt(d2)) / (2.0 * a);
		
		// Choose the smallest t value within the valid range, and check if it's smaller than the value
		// that's already in outHit. If so, return true
		int chosenIdx = SelectSmallestInRange(tvals, tMin, tMax);
		if (chosenIdx != -1) {
			// Try updating the hit result with whichever t value was the smallest in the range
			if (outHit.UpdateTMin(tvals[chosenIdx])) {
				// If it ended up finding a new minT, update the normal value in the hit result
				dvec4 localPos = ray.FindLocAtTime(outHit.t);
				// Normal of hit is just local position - already normalized b/c radius = 1
				// BUT outHit.nor is a vector, so set the w value to 0
				outHit.nor = dvec4(localPos.x, localPos.y, localPos.z, 0);
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

glm::dvec4 Sphere::GetRandomPointOnSurface(double& pdf)
{
	return transf.translation;
}
