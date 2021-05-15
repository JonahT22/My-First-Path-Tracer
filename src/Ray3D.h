#pragma once

#include <glm/glm.hpp>

struct Ray3D {
	Ray3D(glm::dvec4 _start, glm::dvec4 _dir) : start(_start), dir(_dir), invDir(1.0 / _dir) {
		sign[0] = (invDir.x < 0) ? -1 : 1;
		sign[1] = (invDir.y < 0) ? -1 : 1;
		sign[2] = (invDir.z < 0) ? -1 : 1;
	}
	glm::dvec4 start;
	glm::dvec4 dir;
	// Cache reciprocal of direction for ray-box intersections
	glm::dvec4 invDir;
	// Signs of the x, y, and z components of invDir, either -1 or 1 (used in ray-box intersections)
	int sign[3];

	glm::dvec4 FindLocAtTime(double time) {
		return start + (time * dir);
	}
};