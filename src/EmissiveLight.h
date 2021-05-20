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
		obj(_obj),
		sampledLocation(_obj->GetLocation()) {}

	double RandomizeLocation() override {
		// initialize pdf as 1.0, in case the obj doesn't have a randomization point method defined
		double pdf = 1.0;
		// Store the location that was chosen, used for sampling the light
		sampledLocation = obj->GetRandomPointOnSurface(pdf);
		return pdf;
	}

	glm::dvec4 GetLocation() override {
		return sampledLocation;
	}

	glm::dvec3 SampleLight(glm::dvec4 hitLocation) override {
		glm::dvec4 hitVector = hitLocation - sampledLocation;
		double distance = glm::length(hitVector);
		hitVector = glm::normalize(hitVector);
		// Treat this object as a surface that evenly emits light in all directions, attenuated by the angle btwn the surface normal and ray
		double orientationAttenuation = std::max(0.0, glm::dot(hitVector, glm::dvec4(0, -1, 0, 0)));
		return GetColor() * orientationAttenuation * GetDistanceAttenuation(distance);
	}

	std::shared_ptr<SceneObject> GetObject() override {
		return obj;
	}

	glm::dvec3 GetColor() override {
		return obj->GetMaterial()->ke;
	}

private:
	std::shared_ptr<SceneObject> obj = nullptr;
	// The most recently sampled random location of this light source
	glm::dvec4 sampledLocation;
};