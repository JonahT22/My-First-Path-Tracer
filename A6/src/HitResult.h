#pragma once

#include <glm/glm.hpp>
#include <memory>

// Forward declare SceneObject since I need to make a ptr to it
class SceneObject;
struct HitResult {
	// Initilialize as largest possible double value
	double t = std::numeric_limits<double>::max();
	std::shared_ptr<SceneObject> hitObject = nullptr;
	glm::dvec4 loc = glm::dvec4(0, 0, 0, 1);
	glm::dvec4 nor = glm::dvec4(0, 0, 0, 0);
	// TODO: Material properties
	bool UpdateTMin(double newT) {
		if (newT < t) {
			t = newT;
			return true;
		}
		else return false;
	}
};