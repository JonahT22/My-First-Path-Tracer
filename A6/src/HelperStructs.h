#pragma once

#include <glm/glm.hpp>

struct Ray3D {
	glm::vec4 start;
	glm::vec4 dir;
	Ray3D(glm::vec4 _start, glm::vec4 _dir) : start(_start), dir(_dir) {}

	glm::vec4 FindLocAtTime(double time) {
		return start + ((float)time * dir);
	}
};

struct Transform {
	glm::vec4 loc = glm::vec4(0, 0, 0, 1);
	glm::vec3 rot = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	Transform() = default;
	Transform(glm::vec4 _loc, glm::vec3 _rot, glm::vec3 _scale)
		: loc(_loc), rot(_rot), scale(_scale) {}
};

struct HitResult {
	// Initilialize as largest possible double value
	double tMin = std::numeric_limits<double>::max();
	glm::vec4 loc;
	glm::vec4 nor;
	glm::vec3 color;
	// TODO: Material properties
	bool UpdateTMin(double newT) {
		if (newT < tMin) {
			tMin = newT;
			return true;
		}
		else return false;
	}
};