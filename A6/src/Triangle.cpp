#pragma once
#include "Triangle.h"

using namespace glm;
using namespace std;

dvec4 Triangle::BaryInterpNorm(double (&baryCoords)[3]) {
	//TODO remove
	////keep a tally of how much each vertex norm has contributed to the total norm
	//float x = 0, y = 0, z = 0;
	//for (int i = 0; i < 3; i++) {
	//	x += norms[i].x * baryCoords[i];
	//	y += norms[i].y * baryCoords[i];
	//	z += norms[i].z * baryCoords[i];
	//}
	//return dvec4(x, y, z, 0); //construct a vector from the totals
	
	dvec4 interp(0, 0, 0, 0);
	for (int i = 0; i < 3; i++) {
		// Assume this is component-wise multiplication TODO: check that it is
		interp += norms[i] * baryCoords[i];
	}

	interp = normalize(interp);

	return interp;
}

void Triangle::print() {
	std::cout << "Triangle(";
	for (int i = 0; i < 3; i++) {
		std::cout << " <" << locations[i].x << " , " << locations[i].y << " , " << locations[i].z << "> ";
		//std::cout << " <" << norms[i].x << " , " << norms[i].y << " , " << norms[i].z << ">, ";
	}
	std::cout << ")" << std::endl;
}

void Triangle::SetLocations(dvec4& v0, dvec4& v1, dvec4& v2) {
	//set vertex locations
	locations[0] = v0;
	locations[1] = v1;
	locations[2] = v2;
}

void Triangle::SetNorms(dvec4& n0, dvec4& n1, dvec4& n2) {
	norms[0] = n0;
	norms[1] = n1;
	norms[2] = n2;
}

//TODO: remove commented code
bool Triangle::IntersectTriangle(Ray3D& ray, double& t, double& u, double& v) {
	const double EPSILON = 0.000001;
//	double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	dvec3 edge1, edge2, tvec, pvec, qvec;
	double det, inv_det;

	// Find vectors for two edges sharing vert0
	edge1 = locations[1] - locations[0];
	edge2 = locations[2] - locations[0];

	// begin calculating determinant - also used to calculate U parameter
//  CROSS(pvec, dir, edge2);
	pvec = cross(dvec3(ray.dir), edge2);

	// if determinant is near zero, ray lies in plane of triangle
//	det = DOT(edge1, pvec);
	det = dot(edge1, pvec);

	// calculate distance from vert0 to ray origin
//	SUB(tvec, orig, vert0);
	tvec = ray.start - locations[0];
	inv_det = 1.0 / det;

	if (det > EPSILON) {
		// calculate U parameter and test bounds
//		*u = DOT(tvec, pvec);
		u = dot(tvec, pvec);
//		if (*u < 0.0 || *u > det)
		if (u < 0.0 || u > det)
//			return 0;
			return false;

		// prepare to test V parameter
//		CROSS(qvec, tvec, edge1);
		qvec = cross(tvec, edge1);

		// calculate V parameter and test bounds
//		*v = DOT(dir, qvec);
		v = dot(dvec3(ray.dir), qvec);
//		if (*v < 0.0 || *u + *v > det)
		if (v < 0.0 || (u + v) > det)
//			return 0;
			return false;

	}
//	else if (det < -EPSILON)
	else if (det < (-1.0 * EPSILON)) {
		// calculate U parameter and test bounds
//		*u = DOT(tvec, pvec);
		u = dot(tvec, pvec);
//		if (*u > 0.0 || *u < det)
		if (u > 0.0 || u < det)
//			return 0;
			return false;

		// prepare to test V parameter
//		CROSS(qvec, tvec, edge1);
		qvec = cross(tvec, edge1);

		// calculate V parameter and test bounds
//		*v = DOT(dir, qvec);
		v = dot(dvec3(ray.dir), qvec);
//		if (*v > 0.0 || *u + *v < det)
		if (v > 0.0 || (u + v) < det)
//			return 0;
			return false;
	}
//	else return 0;  /* ray is parallell to the plane of the triangle */
	else return false; // ray is parallell to the plane of the triangle

	// calculate t, ray intersects triangle
//	*t = DOT(edge2, qvec) * inv_det;
	t = dot(edge2, qvec) * inv_det;
//	(*u) *= inv_det;
	u *= inv_det;
//	(*v) *= inv_det;
	v *= inv_det;

//	return 1;
	return true;
}


// #define EPSILON 0.000001
///* code rewritten to do tests on the sign of the determinant */
///* the division is before the test of the sign of the det    */
//int intersect_triangle2(double orig[3], double dir[3],
//	double vert0[3], double vert1[3], double vert2[3],
//	double* t, double* u, double* v)
//{
//	double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
//	double det, inv_det;
//
//	/* find vectors for two edges sharing vert0 */
//	SUB(edge1, vert1, vert0);
//	SUB(edge2, vert2, vert0);
//
//	/* begin calculating determinant - also used to calculate U parameter */
//	CROSS(pvec, dir, edge2);
//
//	/* if determinant is near zero, ray lies in plane of triangle */
//	det = DOT(edge1, pvec);
//
//	/* calculate distance from vert0 to ray origin */
//	SUB(tvec, orig, vert0);
//	inv_det = 1.0 / det;
//
//	if (det > EPSILON)
//	{
//		/* calculate U parameter and test bounds */
//		*u = DOT(tvec, pvec);
//		if (*u < 0.0 || *u > det)
//			return 0;
//
//		/* prepare to test V parameter */
//		CROSS(qvec, tvec, edge1);
//
//		/* calculate V parameter and test bounds */
//		*v = DOT(dir, qvec);
//		if (*v < 0.0 || *u + *v > det)
//			return 0;
//
//	}
//	else if (det < -EPSILON)
//	{
//		/* calculate U parameter and test bounds */
//		*u = DOT(tvec, pvec);
//		if (*u > 0.0 || *u < det)
//			return 0;
//
//		/* prepare to test V parameter */
//		CROSS(qvec, tvec, edge1);
//
//		/* calculate V parameter and test bounds */
//		*v = DOT(dir, qvec);
//		if (*v > 0.0 || *u + *v < det)
//			return 0;
//	}
//	else return 0;  /* ray is parallell to the plane of the triangle */
//
//	/* calculate t, ray intersects triangle */
//	*t = DOT(edge2, qvec) * inv_det;
//	(*u) *= inv_det;
//	(*v) *= inv_det;
//
//	return 1;
//}




