#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "SceneObject.h"

class Light {
public:
	Light() = default;
	Light(std::string _name) : name(_name) {}
	virtual glm::dvec3 GetColor() = 0;
	virtual glm::dvec4 GetLocation() = 0;
	// If this light is attached to a sceneobject (i.e. emissive lights), return it. Else, return nullptr
	virtual std::shared_ptr<SceneObject> GetObject() = 0;
	std::string name;
};