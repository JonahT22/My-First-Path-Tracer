#pragma once

#include <glm/glm.hpp>

struct Transform {
	glm::dvec4 loc = glm::dvec4(0, 0, 0, 1);
	glm::dvec3 rot = glm::dvec3(0, 0, 0);
	glm::dvec3 scale = glm::dvec3(1, 1, 1);
	Transform() = default;
	Transform(glm::dvec4 _loc, glm::dvec3 _rot, glm::dvec3 _scale)
		: loc(_loc), rot(_rot), scale(_scale) {}
};
