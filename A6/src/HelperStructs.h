#pragma once

#include <glm/glm.hpp>

struct Ray3D {
	glm::vec4 start;
	glm::vec4 dir;
	Ray3D(glm::vec4 _start, glm::vec4 _dir) : start(_start), dir(_dir) {}
};