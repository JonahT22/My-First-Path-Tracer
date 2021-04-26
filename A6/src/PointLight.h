#pragma once

#include <glm/glm.hpp>
#include <string>

struct PointLight {
	glm::dvec4 pos;
	double intensity;
	std::string name;

	PointLight() :
		pos(glm::dvec4(0, 0, 0, 1)),
		intensity(1.0),
		name("unnamed_light") {}
	PointLight(glm::dvec4 _pos, double _intensity, std::string _name) :
		pos(_pos),
		intensity(_intensity),
		name(_name) {}
};