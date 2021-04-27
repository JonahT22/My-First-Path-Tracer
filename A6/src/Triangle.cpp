#pragma once
#include "Triangle.h"

using namespace glm;
using namespace std;

dvec4 Triangle::baryInterpNorm(double (&baryCoords)[3]) {
	//keep a tally of how much each vertex norm has contributed to the total norm
	float x = 0, y = 0, z = 0;
	for (int i = 0; i < 3; i++) {
		x += norms[i].x * baryCoords[i];
		y += norms[i].y * baryCoords[i];
		z += norms[i].z * baryCoords[i];
	}
	return dvec4(x, y, z, 0); //construct a Vector3D from the totals
}

void Triangle::print()
{
	std::cout << "Triangle(";
	for (int i = 0; i < 3; i++) {
		std::cout << " <" << locations[i].x << " , " << locations[i].y << " , " << locations[i].z << ">, ";
	}
	std::cout << ")" << std::endl;
}

void Triangle::SetLocations(dvec4 v0, dvec4 v1, dvec4 v2)
{
	//set vertex locations
	locations[0] = v0;
	locations[1] = v1;
	locations[2] = v2;
}

void Triangle::SetNorms(dvec4 n0, dvec4 n1, dvec4 n2)
{
	norms[0] = n0;
	norms[1] = n1;
	norms[2] = n2;
}


