#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Light.h"
#include "SceneObject.h"

// Reroutes all light properties to get an emissive object's properties instead
class EmissiveLight : public Light {
public:
	// Call parent constructor
	EmissiveLight(std::string _name, std::shared_ptr<SceneObject> _obj) : Light(_name), obj(_obj) {}

	glm::dvec3 GetColor() override {
		// TODO: make sceneobject materials into a shared ptr
		return obj->GetMaterial().ke;
	}
	glm::dvec4 GetLocation() override {
		return obj->GetRandomPointOnSurface();
	}
	std::shared_ptr<SceneObject> GetObject() override {
		return obj;
	}

private:
	std::shared_ptr<SceneObject> obj = nullptr;
};