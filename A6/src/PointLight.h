#pragma once

#include <glm/glm.hpp>

struct PointLight {
	glm::vec4 pos;
	float intensity;

	PointLight() :
		pos(glm::vec4(0, 0, 0, 1)),
		intensity(1.0) {}
	PointLight(glm::vec4 _pos, float _intensity) :
		pos(_pos),
		intensity(_intensity) {}
};