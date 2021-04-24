#pragma once

#include <glm/glm.hpp>

struct Transform {
	glm::vec4 loc = glm::vec4(0, 0, 0, 1);
	glm::vec3 rot = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	Transform() = default;
	Transform(glm::vec4 _loc, glm::vec3 _rot, glm::vec3 _scale)
		: loc(_loc), rot(_rot), scale(_scale) {}
};
