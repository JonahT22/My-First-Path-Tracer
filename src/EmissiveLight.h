#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Light.h"
#include "SceneObject.h"

// A light that reroutes all property calls to get its emissive object's properties instead
class EmissiveLight : public Light {
public:
	// Call parent constructor
	EmissiveLight(std::string _name, double _L, double _Q, double _falloffDistance, std::shared_ptr<SceneObject> _obj) :
		Light(_name, _L, _Q, _falloffDistance),
		obj(_obj) {}

	glm::dvec3 GetColor() override {
		return obj->GetMaterial()->ke;
	}
	glm::dvec4 GetLocation(double& pdf) override {
		pdf = 1.0;
		return obj->GetRandomPointOnSurface(pdf);
	}
	std::shared_ptr<SceneObject> GetObject() override {
		return obj;
	}

private:
	std::shared_ptr<SceneObject> obj = nullptr;
};