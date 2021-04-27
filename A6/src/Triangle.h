#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "Ray3D.h"

class Triangle {
public:
	Triangle() = default;

	glm::dvec4 baryInterpNorm(double (&baryCoords)[3]);
	void print();
	void SetLocations(glm::dvec4 v0, glm::dvec4 v1, glm::dvec4 v2);
	void SetNorms(glm::dvec4 n0, glm::dvec4 n1, glm::dvec4 n2);
	bool IntersectTriangle(Ray3D ray, glm::dvec4 verts[3], double& t, double& u, double& v);

private:
	glm::dvec4 locations[3];
	glm::dvec4 norms[3];
};
