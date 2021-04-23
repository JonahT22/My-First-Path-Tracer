#pragma once
#include "Sphere.h"

using namespace glm;

bool Sphere::IntersectLocal(Ray3D ray, HitResult& outHit) {
	// TODO: compute normals
	// Assume sphere is at origin with radius 1
	double a = dot(vec3(ray.dir), vec3(ray.dir));
	double b = 2.0 * dot(vec3(ray.dir), vec3(ray.start));
	double c = dot(vec3(ray.start), vec3(ray.start)) - 1.0;
	double d2 = pow(b, 2) - (4 * a * c);
	if (d2 > 0) {
		double t1 = (-1.0 * b + sqrt(d2)) / (2.0 * a);
		double t2 = (-1.0 * b - sqrt(d2)) / (2.0 * a);
		if (t1 > 0 && t1 < t2) {
			//cout << "Hit at time " << t1 << endl;
			return true;
		}
		else if (t2 > 0 && t2 < t1) {
			//cout << "Hit at time " << t1 << endl;
			return true;
		}
		else return false;
	}
	// TODO: remove this? 
	else if (d2 == 0) { // Very rare, almost impossible
		double t = -1.0 * b / (2.0 * a);
		//cout << "Hit at time " << t << endl;
		return true;
	}
	else return false;
	// Better way to organize: return false at the top. If makes it past that, assume there are 2 hits.
	// (if d = 0, then both t's will be the same). If the smaller positive t is less than the one in the 
	// hitresult, then update the hitresult before returning true
	// Maybe: only return true if I actually updated the value of outHit?
}