#pragma once

#include <glm/glm.hpp>
#include <string>

struct PointLight {
	std::string name;
	glm::dvec4 pos;
	double intensity;

	PointLight() :
		name("unnamed_light"),
		pos(glm::dvec4(0, 0, 0, 1)),
		intensity(1.0) {}
	PointLight(std::string _name, glm::dvec4 _pos, double _intensity) :
		name(_name),
		pos(_pos),
		intensity(_intensity) {}
};