#pragma once

#include <glm/glm.hpp>

struct Material {
	glm::vec3 kd;
	glm::vec3 ks;
	glm::vec3 ka;
	double exp;

	Material() :
		kd(glm::vec3(1.0, 1.0, 1.0)),
		ks(glm::vec3(1.0, 1.0, 1.0)),
		ka(glm::vec3(0.1, 0.1, 0.1)),
		exp(100.0)
	{}
	Material(glm::vec3 _kd, glm::vec3 _ks, glm::vec3 _ka, double _exp) :
		kd(_kd),
		ks(_ks),
		ka(_ka),
		exp(_exp)
	{}
};