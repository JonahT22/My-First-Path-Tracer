#pragma once
#include "Triangle.h"

using namespace glm;
using namespace std;

dvec4 Triangle::BaryInterpNorm(double (&baryCoords)[3]) {
	dvec4 interp(0, 0, 0, 0);
	for (int i = 0; i < 3; i++) {
		interp += norms[i] * baryCoords[i];
	}
	interp = normalize(interp);
	return interp;
}

void Triangle::print() {
	std::cout << "Triangle(";
	for (int i = 0; i < 3; i++) {
		std::cout << " <" << locations[i].x << " , " << locations[i].y << " , " << locations[i].z << "> ";
	}
	std::cout << ")" << std::endl;
}

void Triangle::SetLocations(dvec4& v0, dvec4& v1, dvec4& v2) {
	locations[0] = v0;
	locations[1] = v1;
	locations[2] = v2;
}

void Triangle::SetNorms(dvec4& n0, dvec4& n1, dvec4& n2) {
	norms[0] = n0;
	norms[1] = n1;
	norms[2] = n2;
}

bool Triangle::IntersectTriangle(Ray3D& ray, double& t, double& u, double& v) {
	const double EPSILON = 0.000001;
	dvec3 edge1, edge2, tvec, pvec, qvec;
	double det, inv_det;

	// Find vectors for two edges sharing vert0
	edge1 = locations[1] - locations[0];
	edge2 = locations[2] - locations[0];

	// begin calculating determinant - also used to calculate U parameter
	pvec = cross(dvec3(ray.dir), edge2);

	// if determinant is near zero, ray lies in plane of triangle
	det = dot(edge1, pvec);

	// calculate distance from vert0 to ray origin
	tvec = ray.start - locations[0];
	inv_det = 1.0 / det;

	if (det > EPSILON) {
		// calculate U parameter and test bounds
		u = dot(tvec, pvec);
		if (u < 0.0 || u > det)
			return false;

		// prepare to test V parameter
		qvec = cross(tvec, edge1);

		// calculate V parameter and test bounds
		v = dot(dvec3(ray.dir), qvec);
		if (v < 0.0 || (u + v) > det)
			return false;

	}
	else if (det < (-1.0 * EPSILON)) {
		// calculate U parameter and test bounds
		u = dot(tvec, pvec);
		if (u > 0.0 || u < det)
			return false;

		// prepare to test V parameter
		qvec = cross(tvec, edge1);

		// calculate V parameter and test bounds
		v = dot(dvec3(ray.dir), qvec);
		if (v > 0.0 || (u + v) < det)
			return false;
	}
	else return false; // ray is parallel to the plane of the triangle

	// calculate t, ray intersects triangle
	t = dot(edge2, qvec) * inv_det;
	u *= inv_det;
	v *= inv_det;

	return true;
}
