#pragma once

#include <glm/glm.hpp>

struct Transform {
	glm::dvec4 loc = glm::dvec4(0, 0, 0, 1);
	glm::dvec3 rot = glm::dvec3(0, 0, 0);
	glm::dvec3 scale = glm::dvec3(1, 1, 1);
	Transform() = default;
	// (give rotation in degrees)
	Transform(glm::dvec4 _loc, glm::dvec3 _rot, glm::dvec3 _scale)
		: loc(_loc), rot(glm::pi<double>() * _rot / 180.0), scale(_scale) {}
};
