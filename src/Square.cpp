#pragma once
#include "Square.h"

using namespace std;
using namespace glm;

bool Square::IntersectLocal(Ray3D& ray, HitResult& outHit, double tMin, double tMax) {
	// Just like the sphere, the ray will be transformed into the local space of this square, so
	// assume square is at the origin, parallel with x-z plane, with bounds from -0.5 to 0.5 in x and z

	// Full equation: t = dot(n, (planeLoc - ray.start)) / dot(n, ray.dir)
	// Since we're in local space, assume n = <0, 1, 0> and planeLoc = <0, 0, 0>
	double numerator = (-1.0 * ray.start).y;
	double denom = ray.dir.y;

	// Rare, but make sure we don't have a divide-by-zero before calculating t
	if (denom != 0) {
		double newT = numerator / denom;
		if (newT > tMin && newT < tMax) {
			// Check if the hit location is within the bounds of a unit square
			if (IsInUnitSquare(ray.FindLocAtTime(newT))) {
				// Try updating the HitResult 
				if (outHit.UpdateTMin(newT)) {
					// If the HitResult ended up finding a new minT, update the normal value in the hit result and return true
					outHit.nor = dvec4(0, 1, 0, 0);
					return true;
				}
			}
		}
	}

	// return false if:
	// - Denominator is zero (Ray is parallel to plane)
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated w/a new value)
	return false;
}

glm::dvec4 Square::GetRandomPointOnSurface(double& pdf, dvec4& normal)
{
	// Generate random numbers between -0.5 and 0.5
	double randX = (rand() / (double)RAND_MAX) - 0.5;
	double randZ = (rand() / (double)RAND_MAX) - 0.5;
	// PDF = 1/area, since this is a uniform distribution
	pdf = 1.0 / (transf.scale.x * transf.scale.z);
	// Normal = +y in local space (TODO: I can cache this)
	normal = invTranspMtx * dvec4(0, 1, 0, 0);
	normal.w = 0.0;
	normal = normalize(normal);
	return modelMtx * dvec4(randX, 0, randZ, 1);
}

bool Square::IsInUnitSquare(const glm::vec4& v) const {
	return (v.x > -0.5 && v.x < 0.5) && (v.z > -0.5 && v.z < 0.5);
}
