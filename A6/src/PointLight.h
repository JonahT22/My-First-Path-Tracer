#pragma once

#include <glm/glm.hpp>

struct PointLight {
	glm::dvec4 pos;
	double intensity;

	PointLight() :
		pos(glm::dvec4(0, 0, 0, 1)),
		intensity(1.0) {}
	PointLight(glm::dvec4 _pos, double _intensity) :
		pos(_pos),
		intensity(_intensity) {}
};