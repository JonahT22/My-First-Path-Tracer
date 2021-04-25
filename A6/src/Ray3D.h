#pragma once

#include <glm/glm.hpp>

struct Ray3D {
	glm::dvec4 start;
	glm::dvec4 dir;
	Ray3D(glm::dvec4 _start, glm::dvec4 _dir) : start(_start), dir(_dir) {}

	glm::dvec4 FindLocAtTime(double time) {
		return start + (time * dir);
	}
};