#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "Light.h"

// An infinitely small light with position and color
struct PointLight : public Light {
	// Call parent constructor
	PointLight(std::string _name, double _L, double _Q, double _falloffDistance, glm::dvec4 _loc, glm::dvec3 _color) :
		Light(_name, _L, _Q, _falloffDistance), 
		loc(_loc),
		color(_color) {}

	glm::dvec3 SampleLight(glm::dvec4 hitLocation) override {
		double distance = glm::length(hitLocation - loc);
		return GetColor() * GetDistanceAttenuation(distance);
	}

	double RandomizeLocation() override {
		// Not randomly sampling location, so pdf is just 1
		return 1.0;
	}

	glm::dvec4 GetLocation() override {
		return loc;
	}
	// Returns null, since point lights are not attached to a particular object
	std::shared_ptr<SceneObject> GetObject() override {
		return nullptr;
	}

	glm::dvec3 GetColor() override {
		return color;
	}
private:
	glm::dvec4 loc = glm::dvec4(0, 0, 0, 1);
	glm::dvec3 color = glm::dvec3(1, 1, 1);
};