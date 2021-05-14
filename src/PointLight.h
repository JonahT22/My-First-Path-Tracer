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

	glm::dvec3 GetColor() override {
		// TODO: make sceneobject materials into a shared ptr
		return color;
	}
	glm::dvec4 GetLocation() override {
		return loc;
	}
	// Returns null, since point lights are not attached to a particular object
	std::shared_ptr<SceneObject> GetObject() override {
		return nullptr;
	}
private:
	glm::dvec4 loc = glm::dvec4(0, 0, 0, 1);
	glm::dvec3 color = glm::dvec3(1, 1, 1);
};