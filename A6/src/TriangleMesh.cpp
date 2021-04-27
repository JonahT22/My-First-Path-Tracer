#pragma once
#include "TriangleMesh.h"

using namespace std;
using namespace glm;

TriangleMesh::TriangleMesh(std::string _name, Transform _transf, Material _mat)
	: SceneObject(_name, _transf, _mat) {
	Init();
}

bool TriangleMesh::IntersectLocal(Ray3D ray, HitResult& outHit, double tMin, double tMax) {
	// Run a collision check against the bounding sphere, immediately exit if it did not hit
	// TODO: set up the bounding sphere
	if (!boundingSphere->Hit(ray, outHit)) {
		return false;
	}
	// Iterate over each triangle in the list, running its intersection function
	// Get the t value from the intersection function, and check if its within the bounds/smaller than hit's minT
	for (auto& tri : allTriangles) {

	}
	// If so, run barycentric interpolation to get the normal and store in hitresult

	return true;
	// return false if:
	// - discriminant was negative
	// - No t values were found within the given range
	// - No valid t values were found that were less than the hitResult's tmin (hitresult wasn't updated)
	//return false;
}

void TriangleMesh::Init() {
	// TODO: remove magic number
	double boundRadius = 2.0;
	boundingSphere = make_unique<Sphere>(
		"Bounding_Sphere",
		Transform(dvec4(0, 0, 0, 1), dvec3(0, 0, 0), dvec3(boundRadius, boundRadius, boundRadius)),
		Material(dvec3(0, 0, 0), dvec3(0, 0, 0), dvec3(0.1, 0.1, 0.1), 1, 0)
		);
	PopulateTriangleArray("../resources/tri.obj");
}

void TriangleMesh::PopulateTriangleArray(std::string filename) {
}
