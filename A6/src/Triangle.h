#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "Ray3D.h"

class Triangle {
public:
	Triangle() = default;

	glm::dvec4 BaryInterpNorm(double(&baryCoords)[3]);
	void print();
	void SetLocations(glm::dvec4& v0, glm::dvec4& v1, glm::dvec4& v2);
	void SetNorms(glm::dvec4& n0, glm::dvec4& n1, glm::dvec4& n2);
	// Test the provided ray against this triangle's location vectors. Returns distance and barycentric coords in t, u, v
	bool IntersectTriangle(Ray3D& ray, double& t, double& u, double& v);

private:
	glm::dvec4 locations[3];
	glm::dvec4 norms[3];
};
